#ifndef BLE_PROVISIONING_H
#define BLE_PROVISIONING_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/aes.h>
#include "Config.h"
#include "SerialLogger.h"
#include "WiFiManager.h"
#include "NVSManager.h"

// ============================================================================
// BLEProvisioning - Secure WiFi Provisioning via BLE with ECDH
// ============================================================================
// Features:
// - BLE GATT server for WiFi configuration
// - ECDH P-256 key exchange for secure communication
// - AES-128 password encryption
// - WiFi network scanning
// - Connection status notifications
// ============================================================================

class BLEProvisioning {
public:
    // Provisioning state
    enum ProvisioningState {
        STATE_IDLE,
        STATE_ADVERTISING,
        STATE_CONNECTED,
        STATE_KEY_EXCHANGED,
        STATE_CREDENTIALS_RECEIVED,
        STATE_CONNECTING,
        STATE_SUCCESS,
        STATE_FAILED
    };
    
    // Initialize BLE provisioning
    static bool begin() {
        LOG_SECTION("Initializing BLE Provisioning");
        
        // Get device name with MAC suffix
        deviceName = String(BLE_DEVICE_NAME_PREFIX) + "-" + WiFiManager::getMacSuffix();
        LOG_PRINTF("INFO ", "  BLE Device: %s", deviceName.c_str());
        
        // Initialize BLE
        BLEDevice::init(deviceName.c_str());
        BLEDevice::setMTU(BLE_MTU_SIZE);
        
        // Create BLE server
        pServer = BLEDevice::createServer();
        pServer->setCallbacks(new ServerCallbacks());
        
        // Initialize ECDH crypto
        if (!initCrypto()) {
            LOG_ERROR("Failed to initialize cryptography!");
            return false;
        }
        
        // Create services
        createKeyExchangeService();
        createWiFiScanService();
        createCredentialService();
        
        // Start advertising
        startAdvertising();
        
        currentState = STATE_ADVERTISING;
        LOG_INFO("BLE Provisioning ready!");
        
        return true;
    }
    
    // Start BLE advertising
    static void startAdvertising() {
        BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
        
        pAdvertising->addServiceUUID(KEY_EXCHANGE_SERVICE_UUID);
        pAdvertising->addServiceUUID(WIFI_SCAN_SERVICE_UUID);
        pAdvertising->addServiceUUID(CREDENTIAL_SERVICE_UUID);
        
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06);  // Functions for connection interval
        pAdvertising->setMaxPreferred(0x12);
        
        BLEDevice::startAdvertising();
        LOG_INFO("BLE advertising started");
    }
    
    // Stop BLE advertising
    static void stopAdvertising() {
        BLEDevice::getAdvertising()->stop();
        LOG_INFO("BLE advertising stopped");
    }
    
    // Get current state
    static ProvisioningState getState() {
        return currentState;
    }
    
    // Check if client connected
    static bool isConnected() {
        return deviceConnected;
    }
    
    // Check if provisioning completed
    static bool isProvisioningCompleted() {
        return provisioningCompleted;
    }


private:
    static BLEServer* pServer;
    static String deviceName;
    static ProvisioningState currentState;
    static bool deviceConnected;
    static bool provisioningCompleted; // Flag to prevent BLE restart after success
    
    // ECDH Crypto variables
    static mbedtls_ecdh_context ecdhContext;
    static mbedtls_entropy_context entropy;
    static mbedtls_ctr_drbg_context ctrDrbg;
    static uint8_t sharedSecret[SECURITY_ECDH_KEY_SIZE];
    static uint8_t ourPublicKey[128];  // Buffer for our public key
    static size_t ourPublicKeyLen;
    static bool keyExchangeComplete;
    
    // Credential storage
    static String receivedSSID;
    static String receivedPassword;
    
    // BLE Characteristics
    static BLECharacteristic* pPublicKeyESP;
    static BLECharacteristic* pPublicKeyApp;
    static BLECharacteristic* pWiFiScanTrigger;
    static BLECharacteristic* pWiFiScanResults;
    static BLECharacteristic* pCredentialSSID;
    static BLECharacteristic* pCredentialPass;
    static BLECharacteristic* pCredentialConnect;
    static BLECharacteristic* pCredentialStatus;
    
    // ========================================================================
    // Cryptography Functions
    // ========================================================================
    
    // Initialize ECDH crypto
    static bool initCrypto() {
        LOG_INFO("Initializing ECDH P-256 cryptography...");
        
        // Initialize contexts
        mbedtls_ecdh_init(&ecdhContext);
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_init(&ctrDrbg);
        
        // Seed random number generator
        const char* pers = "pixeltree_ecdh";
        int ret = mbedtls_ctr_drbg_seed(
            &ctrDrbg,
            mbedtls_entropy_func,
            &entropy,
            (const unsigned char*)pers,
            strlen(pers)
        );
        
        if (ret != 0) {
            LOG_PRINTF("ERROR", "mbedtls_ctr_drbg_seed failed: %d", ret);
            return false;
        }
        
        // Setup ECDH context with P-256 curve using new API
        ret = mbedtls_ecdh_setup(&ecdhContext, MBEDTLS_ECP_DP_SECP256R1);
        if (ret != 0) {
            LOG_PRINTF("ERROR", "mbedtls_ecdh_setup failed: %d", ret);
            return false;
        }
        
        // Generate our key pair using make_params
        ret = mbedtls_ecdh_make_params(
            &ecdhContext,
            &ourPublicKeyLen,
            ourPublicKey,
            sizeof(ourPublicKey),
            mbedtls_ctr_drbg_random,
            &ctrDrbg
        );
        
        if (ret != 0) {
            LOG_PRINTF("ERROR", "mbedtls_ecdh_make_params failed: %d", ret);
            return false;
        }
        
        LOG_INFO("ECDH key pair generated successfully");
        keyExchangeComplete = false;
        
        return true;
    }
    
    // Get our public key for transmission
    static String getPublicKey() {
        // Convert to hex string (skip first 3 bytes which are ASN.1 overhead)
        // Our public key is stored in ourPublicKey buffer
        String hexKey = "";
        
        // Find the actual public key in the buffer (65 bytes uncompressed point)
        // The buffer contains: [curve_params][public_key]
        // For P-256, public key starts after initial bytes
        
        size_t keyStart = ourPublicKeyLen - 65;  // Last 65 bytes are the public key
        
        for (size_t i = keyStart; i < ourPublicKeyLen; i++) {
            char hex[3];
            snprintf(hex, sizeof(hex), "%02X", ourPublicKey[i]);
            hexKey += hex;
        }
        
        return hexKey;
    }
    
    // Process received public key from app
    static bool processAppPublicKey(const String& hexKey) {
        LOG_INFO("Processing app public key...");
        
        // Convert hex string to bytes (65 bytes uncompressed point)
        if (hexKey.length() != 130) { // 65 bytes = 130 hex chars
            LOG_ERROR("Invalid public key length!");
            return false;
        }
        
        uint8_t peerPublicKey[65];
        for (int i = 0; i < 65; i++) {
            String byteStr = hexKey.substring(i * 2, i * 2 + 2);
            peerPublicKey[i] = strtoul(byteStr.c_str(), NULL, 16);
        }
        
        // Verify it's an uncompressed point (starts with 0x04)
        if (peerPublicKey[0] != 0x04) {
            LOG_ERROR("Invalid point format - must be uncompressed (0x04)");
            return false;
        }
        
        // Create a temporary keypair structure with just the peer's public key
        mbedtls_ecp_keypair peerKeypair;
        mbedtls_ecp_keypair_init(&peerKeypair);
        
        // Load the curve parameters (P-256)
        int ret = mbedtls_ecp_group_load(&peerKeypair.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1);
        if (ret != 0) {
            LOG_PRINTF("ERROR", "Failed to load curve: %d", ret);
            mbedtls_ecp_keypair_free(&peerKeypair);
            return false;
        }
        
        // Import the peer's public key point
        ret = mbedtls_ecp_point_read_binary(
            &peerKeypair.MBEDTLS_PRIVATE(grp),
            &peerKeypair.MBEDTLS_PRIVATE(Q),
            peerPublicKey,
            65
        );
        
        if (ret != 0) {
            LOG_PRINTF("ERROR", "mbedtls_ecp_point_read_binary failed: %d", ret);
            mbedtls_ecp_keypair_free(&peerKeypair);
            return false;
        }
        
        // Import peer's public key into ECDH context using official API
        ret = mbedtls_ecdh_get_params(&ecdhContext, &peerKeypair, MBEDTLS_ECDH_THEIRS);
        mbedtls_ecp_keypair_free(&peerKeypair);
        
        if (ret != 0) {
            LOG_PRINTF("ERROR", "mbedtls_ecdh_get_params failed: %d", ret);
            return false;
        }
        
        // Compute shared secret
        size_t secretLen;
        ret = mbedtls_ecdh_calc_secret(
            &ecdhContext,
            &secretLen,
            sharedSecret,
            SECURITY_ECDH_KEY_SIZE,
            mbedtls_ctr_drbg_random,
            &ctrDrbg
        );
        
        if (ret != 0) {
            LOG_PRINTF("ERROR", "mbedtls_ecdh_calc_secret failed: %d", ret);
            return false;
        }
        
        keyExchangeComplete = true;
        currentState = STATE_KEY_EXCHANGED;
        LOG_INFO("Shared secret computed - key exchange complete!");
        
        return true;
    }
    
    // Decrypt password using AES-128 with shared secret
    static String decryptPassword(const String& encryptedHex) {
        if (!keyExchangeComplete) {
            LOG_ERROR("Cannot decrypt - key exchange not complete!");
            return "";
        }
        
        LOG_INFO("Decrypting password...");
        
        // Convert hex to bytes
        size_t encLen = encryptedHex.length() / 2;
        uint8_t* encrypted = new uint8_t[encLen];
        
        for (size_t i = 0; i < encLen; i++) {
            String byteStr = encryptedHex.substring(i * 2, i * 2 + 2);
            encrypted[i] = strtoul(byteStr.c_str(), NULL, 16);
        }
        
        // Initialize AES context
        mbedtls_aes_context aes;
        mbedtls_aes_init(&aes);
        
        // Set decryption key (first 16 bytes of shared secret)
        int ret = mbedtls_aes_setkey_dec(&aes, sharedSecret, 128);
        if (ret != 0) {
            LOG_PRINTF("ERROR", "AES setkey failed: %d", ret);
            delete[] encrypted;
            mbedtls_aes_free(&aes);
            return "";
        }
        
        // Decrypt (assuming ECB mode for simplicity, production should use CBC/GCM)
        uint8_t* decrypted = new uint8_t[encLen];
        
        for (size_t i = 0; i < encLen; i += 16) {
            ret = mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, encrypted + i, decrypted + i);
            if (ret != 0) {
                LOG_PRINTF("ERROR", "AES decrypt failed: %d", ret);
                delete[] encrypted;
                delete[] decrypted;
                mbedtls_aes_free(&aes);
                return "";
            }
        }
        
        // Remove PKCS#7 padding
        uint8_t padding = decrypted[encLen - 1];
        size_t decryptedLen = encLen - padding;
        
        // Convert to string
        String password = "";
        for (size_t i = 0; i < decryptedLen; i++) {
            password += (char)decrypted[i];
        }
        
        // Cleanup
        delete[] encrypted;
        delete[] decrypted;
        mbedtls_aes_free(&aes);
        
        LOG_INFO("Password decrypted successfully");
        return password;
    }
    
    // ========================================================================
    // BLE Service Creation
    // ========================================================================
    
    // Create Key Exchange Service
    static void createKeyExchangeService() {
        BLEService* pService = pServer->createService(KEY_EXCHANGE_SERVICE_UUID);
        
        // ESP Public Key (Read)
        pPublicKeyESP = pService->createCharacteristic(
            PUBLIC_KEY_ESP_UUID,
            BLECharacteristic::PROPERTY_READ
        );
        pPublicKeyESP->setValue(getPublicKey().c_str());
        
        // App Public Key (Write)
        pPublicKeyApp = pService->createCharacteristic(
            PUBLIC_KEY_APP_UUID,
            BLECharacteristic::PROPERTY_WRITE
        );
        pPublicKeyApp->setCallbacks(new PublicKeyCallbacks());
        
        pService->start();
        LOG_INFO("Key Exchange Service created");
    }
    
    // Create WiFi Scan Service
    static void createWiFiScanService() {
        BLEService* pService = pServer->createService(WIFI_SCAN_SERVICE_UUID);
        
        // Scan Trigger (Write)
        pWiFiScanTrigger = pService->createCharacteristic(
            WIFI_SCAN_TRIGGER_UUID,
            BLECharacteristic::PROPERTY_WRITE
        );
        pWiFiScanTrigger->setCallbacks(new ScanTriggerCallbacks());
        
        // Scan Results (Read)
        pWiFiScanResults = pService->createCharacteristic(
            WIFI_SCAN_RESULTS_UUID,
            BLECharacteristic::PROPERTY_READ
        );
        
        pService->start();
        LOG_INFO("WiFi Scan Service created");
    }
    
    // Create Credential Service
    static void createCredentialService() {
        BLEService* pService = pServer->createService(CREDENTIAL_SERVICE_UUID);
        
        // SSID (Write)
        pCredentialSSID = pService->createCharacteristic(
            CREDENTIAL_SSID_UUID,
            BLECharacteristic::PROPERTY_WRITE
        );
        pCredentialSSID->setCallbacks(new SSIDCallbacks());
        
        // Encrypted Password (Write)
        pCredentialPass = pService->createCharacteristic(
            CREDENTIAL_PASS_UUID,
            BLECharacteristic::PROPERTY_WRITE
        );
        pCredentialPass->setCallbacks(new PasswordCallbacks());
        
        // Connect Trigger (Write)
        pCredentialConnect = pService->createCharacteristic(
            CREDENTIAL_CONNECT_UUID,
            BLECharacteristic::PROPERTY_WRITE
        );
        pCredentialConnect->setCallbacks(new ConnectCallbacks());
        
        // Connection Status (Notify)
        pCredentialStatus = pService->createCharacteristic(
            CREDENTIAL_STATUS_UUID,
            BLECharacteristic::PROPERTY_NOTIFY
        );
        pCredentialStatus->addDescriptor(new BLE2902());
        
        pService->start();
        LOG_INFO("Credential Service created");
    }
    
    // ========================================================================
    // BLE Callbacks
    // ========================================================================
    
    // Server connection callbacks
    class ServerCallbacks : public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) {
            deviceConnected = true;
            currentState = STATE_CONNECTED;
            LOG_INFO("BLE client connected");
        }
        
        void onDisconnect(BLEServer* pServer) {
            deviceConnected = false;
            LOG_INFO("BLE client disconnected");
            
            // Only restart advertising if provisioning hasn't completed
            if (!provisioningCompleted) {
                currentState = STATE_ADVERTISING;
                LOG_INFO("Restarting BLE advertising...");
                delay(500);
                startAdvertising();
                
                // Also restart AP in case it was stopped during WiFi scan
                LOG_INFO("Ensuring AP is active...");
                WiFiManager::startAP();
            } else {
                LOG_INFO("Provisioning completed - BLE will not restart");
            }
        }
    };
    
    // Public Key Write Callback
    class PublicKeyCallbacks : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic* pCharacteristic) {
            String value = pCharacteristic->getValue().c_str();
            LOG_PRINTF("INFO ", "Received app public key (%d chars)", value.length());
            
            if (processAppPublicKey(value)) {
                LOG_INFO("Key exchange successful!");
            } else {
                LOG_ERROR("Key exchange failed!");
            }
        }
    };
    
    // WiFi Scan Trigger Callback
    class ScanTriggerCallbacks : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic* pCharacteristic) {
            String value = pCharacteristic->getValue().c_str();
            
            if (value == "1") {
                LOG_INFO("WiFi scan triggered via BLE");
                String results = WiFiManager::scanNetworks();
                pWiFiScanResults->setValue(results.c_str());
                LOG_INFO("Scan results ready for read");
            }
        }
    };
    
    // SSID Write Callback
    class SSIDCallbacks : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic* pCharacteristic) {
            receivedSSID = pCharacteristic->getValue().c_str();
            LOG_PRINTF("INFO ", "Received SSID: %s", receivedSSID.c_str());
        }
    };
    
    // Password Write Callback
    class PasswordCallbacks : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic* pCharacteristic) {
            String encryptedPassword = pCharacteristic->getValue().c_str();
            LOG_INFO("Received encrypted password");
            
            receivedPassword = decryptPassword(encryptedPassword);
            
            if (receivedPassword.isEmpty()) {
                LOG_ERROR("Password decryption failed!");
                currentState = STATE_FAILED;
            } else {
                LOG_INFO("Password decrypted successfully");
                currentState = STATE_CREDENTIALS_RECEIVED;
            }
        }
    };
    
    // Connect Trigger Callback
    class ConnectCallbacks : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic* pCharacteristic) {
            String value = pCharacteristic->getValue().c_str();
            
            if (value == "1" && currentState == STATE_CREDENTIALS_RECEIVED) {
                LOG_INFO("Connection request received via BLE");
                currentState = STATE_CONNECTING;
                
                // Clear any previous status before attempting connection
                pCredentialStatus->setValue("");
                
                // Attempt WiFi connection - returns detailed result
                WiFiManager::ConnectionResult result = 
                    WiFiManager::connectStation(receivedSSID, receivedPassword);
                
                // Handle result based on connection outcome
                switch (result) {
                    case WiFiManager::CONN_SUCCESS:
                        // Success - save credentials and finish
                        NVSManager::saveCredentials(receivedSSID, receivedPassword);
                        currentState = STATE_SUCCESS;
                        pCredentialStatus->setValue("SUCCESS");
                        pCredentialStatus->notify();
                        LOG_INFO("Provisioning successful!");
                        
                        provisioningCompleted = true;
                        
                        delay(1000); // Give time for notification
                        LOG_INFO("Stopping BLE advertising - provisioning complete");
                        BLEProvisioning::stopAdvertising();
                        break;
                        
                    case WiFiManager::CONN_WRONG_PASSWORD:
                        // Wrong password - stay ready for retry!
                        currentState = STATE_CREDENTIALS_RECEIVED;
                        pCredentialStatus->setValue("WRONG_PASSWORD");
                        pCredentialStatus->notify();
                        delay(100); // Give time for notification to be sent
                        pCredentialStatus->setValue(""); // Clear for next attempt
                        LOG_WARN("Wrong password - ready for new attempt");
                        break;
                        
                    case WiFiManager::CONN_SSID_NOT_FOUND:
                        // Network disappeared - ready for retry
                        currentState = STATE_CREDENTIALS_RECEIVED;
                        pCredentialStatus->setValue("SSID_NOT_FOUND");
                        pCredentialStatus->notify();
                        delay(100);
                        pCredentialStatus->setValue(""); // Clear for next attempt
                        LOG_WARN("SSID not found - ready for new attempt");
                        break;
                        
                    case WiFiManager::CONN_AUTH_FAILED:
                        // Auth failed - ready for retry
                        currentState = STATE_CREDENTIALS_RECEIVED;
                        pCredentialStatus->setValue("AUTH_FAILED");
                        pCredentialStatus->notify();
                        delay(100);
                        pCredentialStatus->setValue(""); // Clear for next attempt
                        LOG_WARN("Auth failed - ready for new attempt");
                        break;
                        
                    case WiFiManager::CONN_TIMEOUT:
                    default:
                        // Timeout or unknown - ready for retry
                        currentState = STATE_CREDENTIALS_RECEIVED;
                        pCredentialStatus->setValue("TIMEOUT");
                        pCredentialStatus->notify();
                        delay(100);
                        pCredentialStatus->setValue(""); // Clear for next attempt
                        LOG_WARN("Timeout - ready for new attempt");
                        break;
                }
            }
        }
    };

};


// Static member initialization
BLEServer* BLEProvisioning::pServer = nullptr;
String BLEProvisioning::deviceName = "";
BLEProvisioning::ProvisioningState BLEProvisioning::currentState = BLEProvisioning::STATE_IDLE;
bool BLEProvisioning::deviceConnected = false;
bool BLEProvisioning::provisioningCompleted = false;

mbedtls_ecdh_context BLEProvisioning::ecdhContext;
mbedtls_entropy_context BLEProvisioning::entropy;
mbedtls_ctr_drbg_context BLEProvisioning::ctrDrbg;
uint8_t BLEProvisioning::sharedSecret[SECURITY_ECDH_KEY_SIZE];
uint8_t BLEProvisioning::ourPublicKey[128];
size_t BLEProvisioning::ourPublicKeyLen = 0;
bool BLEProvisioning::keyExchangeComplete = false;

String BLEProvisioning::receivedSSID = "";
String BLEProvisioning::receivedPassword = "";

BLECharacteristic* BLEProvisioning::pPublicKeyESP = nullptr;
BLECharacteristic* BLEProvisioning::pPublicKeyApp = nullptr;
BLECharacteristic* BLEProvisioning::pWiFiScanTrigger = nullptr;
BLECharacteristic* BLEProvisioning::pWiFiScanResults = nullptr;
BLECharacteristic* BLEProvisioning::pCredentialSSID = nullptr;
BLECharacteristic* BLEProvisioning::pCredentialPass = nullptr;
BLECharacteristic* BLEProvisioning::pCredentialConnect = nullptr;
BLECharacteristic* BLEProvisioning::pCredentialStatus = nullptr;

#endif // BLE_PROVISIONING_H
