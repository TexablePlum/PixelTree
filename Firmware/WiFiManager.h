#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "Config.h"
#include "SerialLogger.h"

// ============================================================================
// WiFiManager - Dual Mode WiFi Handler (AP + Station)
// ============================================================================
// Features:
// - Access Point mode with MAC-based SSID
// - Station mode with intelligent error detection
// - Event-based connection monitoring (detects wrong password quickly!)
// - Automatic fallback to AP
// - Connection status monitoring
// ============================================================================



class WiFiManager {
public:
    enum WiFiMode {
        MODE_NONE,
        MODE_AP,
        MODE_STATION,
        MODE_AP_STATION  // Both modes simultaneously
    };
    
    // Connection result enum for detailed error reporting
    enum ConnectionResult {
        CONN_SUCCESS,           // Connected successfully
        CONN_WRONG_PASSWORD,    // Wrong password (4WAY_HANDSHAKE_TIMEOUT)
        CONN_SSID_NOT_FOUND,    // Network not found / disappeared
        CONN_AUTH_FAILED,       // Other authentication failure
        CONN_TIMEOUT            // 30s timeout (backup)
    };
    
    // Initialize WiFi subsystem
    static void begin() {
        LOG_INFO("Initializing WiFi Manager...");
        
        // Set WiFi mode to OFF initially
        WiFi.mode(WIFI_OFF);
        delay(100);
        
        currentMode = MODE_NONE;
        
        // Get MAC address for device naming
        macSuffix = getMacSuffix();
        LOG_PRINTF("INFO ", "Device MAC suffix: %s", macSuffix.c_str());
        
        // Register WiFi event handler once
        registerWiFiEventHandler();
    }
    
    // Start Access Point mode
    static bool startAP() {
        LOG_SECTION("Starting Access Point Mode");
        
        // Build SSID with MAC suffix
        String apSSID = String(AP_SSID_PREFIX) + "-" + macSuffix;
        
        LOG_PRINTF("INFO ", "  AP SSID: %s", apSSID.c_str());
        LOG_INFO("  AP Channel: " + String(AP_CHANNEL));
        LOG_INFO("  No password (open network)");
        
        // Set WiFi mode
        WiFi.mode(WIFI_AP);
        delay(100);
        
        // Configure and start AP
        bool success = WiFi.softAP(
            apSSID.c_str(),
            nullptr,              // No password
            AP_CHANNEL,
            AP_HIDDEN,
            AP_MAX_CONNECTIONS
        );
        
        if (!success) {
            LOG_ERROR("Failed to start Access Point!");
            return false;
        }
        
        // Get AP IP address
        IPAddress apIP = WiFi.softAPIP();
        LOG_PRINTF("INFO ", "  AP IP Address: %s", apIP.toString().c_str());
        
        currentMode = MODE_AP;
        LOG_INFO("Access Point started successfully!");
        
        return true;
    }
    
    // Connect to WiFi as station - returns detailed result
    static ConnectionResult connectStation(const String& ssid, const String& password) {
        LOG_SECTION("Connecting to WiFi Station");
        LOG_PRINTF("INFO ", "  SSID: %s", ssid.c_str());
        LOG_INFO("  Timeout: " + String(WIFI_CONNECT_TIMEOUT_MS / 1000) + "s");
        
        // Reset connection state before attempting
        resetConnectionState();
        
        // Set WiFi mode
        WiFi.mode(WIFI_STA);
        delay(100);
        
        // Set hostname with MAC suffix
        String hostname = String(DEVICE_NAME_PREFIX) + "-" + macSuffix;
        WiFi.setHostname(hostname.c_str());
        
        // Begin connection
        WiFi.begin(ssid.c_str(), password.c_str());
        
        // Wait for connection
        unsigned long startTime = millis();
        int dotCount = 0;
        
        while (!connectionDone) {
            // Check for timeout
            if (millis() - startTime > WIFI_CONNECT_TIMEOUT_MS) {
                LOG_ERROR("WiFi connection timeout (30s)!");
                connectionResult = CONN_TIMEOUT;
                break;
            }
            
            // Print progress dots
            if (dotCount++ % 10 == 0) {
                Serial.print(".");
                if (dotCount >= 50) {
                    Serial.println();
                    dotCount = 0;
                }
            }
            
            delay(100); // Let events process
        }
        
        Serial.println(); // New line after dots
        
        // Handle result
        if (connectionResult == CONN_SUCCESS) {
            LOG_INFO("WiFi connected successfully!");
            LOG_PRINTF("INFO ", "  IP Address: %s", WiFi.localIP().toString().c_str());
            LOG_PRINTF("INFO ", "  Gateway: %s", WiFi.gatewayIP().toString().c_str());
            LOG_PRINTF("INFO ", "  RSSI: %d dBm", WiFi.RSSI());
            currentMode = MODE_STATION;
            
            // Start mDNS responder for service discovery
            if (!MDNS.begin(getDeviceName().c_str())) {
                LOG_ERROR("mDNS failed to start");
            } else {
                // Advertise HTTP service for discovery
                MDNS.addService("http", "tcp", HTTP_SERVER_PORT);
                
                // Add TXT records for device identification
                MDNS.addServiceTxt("http", "tcp", "macSuffix", getMacSuffix());
                MDNS.addServiceTxt("http", "tcp", "deviceName", getDeviceName());
                
                LOG_PRINTF("INFO ", "mDNS responder started: %s.local", getDeviceName().c_str());
                LOG_PRINTF("INFO ", "  Service: _http._tcp, Port: %d", HTTP_SERVER_PORT);
                LOG_PRINTF("INFO ", "  TXT: macSuffix=%s, deviceName=%s", getMacSuffix().c_str(), getDeviceName().c_str());
            }
        } else {
            // Cleanup after failed attempt - ready for retry
            LOG_WARN("Connection failed, cleaning up for retry...");
            WiFi.disconnect(true);  // true = clear credentials from memory
            delay(100);
            WiFi.mode(WIFI_STA);    // Stay in STA mode for next attempt
            currentMode = MODE_NONE;
            LOG_INFO("Ready for next connection attempt");
        }
        
        return connectionResult;
    }
    
    // Scan for available networks
    static String scanNetworks() {
        LOG_INFO("Scanning WiFi networks...");
        
        // Ensure WiFi is in station mode for scanning
        WiFi.mode(WIFI_STA);
        delay(100);
        
        int networkCount = WiFi.scanNetworks();
        
        if (networkCount == 0) {
            LOG_WARN("No WiFi networks found!");
            return "[]";
        }
        
        LOG_PRINTF("INFO ", "Found %d networks", networkCount);
        
        // Build JSON array of networks with dynamic size checking
        // BLE MTU limit: 512 bytes
        const int MAX_PAYLOAD_SIZE = 480; // Safe margin below 512-byte MTU
        String json = "[";
        int networksAdded = 0;
        
        // Networks are already sorted by RSSI (strongest first)
        for (int i = 0; i < networkCount; i++) {
            // Build network JSON entry
            String networkJson = "";
            if (i > 0) networkJson += ",";
            
            networkJson += "{";
            networkJson += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
            networkJson += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
            networkJson += "\"secure\":" + String(WiFi.encryptionType(i) != WIFI_AUTH_OPEN ? "true" : "false");
            networkJson += "}";
            
            // Check if adding this network would exceed payload limit
            int projectedSize = json.length() + networkJson.length() + 1; // +1 for closing ']'
            
            if (projectedSize > MAX_PAYLOAD_SIZE) {
                LOG_PRINTF("WARN ", "Payload limit reached. Sending %d strongest networks (size: %d bytes)", 
                    networksAdded, json.length() + 1);
                break;
            }
            
            // Add network to JSON
            json += networkJson;
            networksAdded++;
            
            // Log each network
            LOG_PRINTF("INFO ", "  [%d] %s (%d dBm) %s", 
                i,
                WiFi.SSID(i).c_str(),
                WiFi.RSSI(i),
                WiFi.encryptionType(i) != WIFI_AUTH_OPEN ? "🔒" : "🔓"
            );
        }
        
        json += "]";
        
        // Clean up scan results
        WiFi.scanDelete();
        
        return json;
    }
    
    // Check if connected to WiFi
    static bool isConnected() {
        return (currentMode == MODE_STATION && WiFi.status() == WL_CONNECTED);
    }
    
    // Get current mode
    static WiFiMode getMode() {
        return currentMode;
    }
    
    // Get device name with MAC suffix
    static String getDeviceName() {
        return String(DEVICE_NAME_PREFIX) + "-" + macSuffix;
    }
    
    // Get MAC suffix for naming
    static String getMacSuffix() {
        // Get MAC address as uint64 from eFuse (works before WiFi init)
        uint64_t macInt = ESP.getEfuseMac();
        
        // Extract last 2 bytes (16 bits) for suffix
        uint16_t suffix16 = (uint16_t)(macInt & 0xFFFF);
        
        // Format as 4 hex characters
        char suffix[5];
        snprintf(suffix, sizeof(suffix), "%04X", suffix16);
        
        return String(suffix);
    }
    
    // Disconnect and stop WiFi
    static void disconnect() {
        LOG_INFO("Disconnecting WiFi...");
        
        if (currentMode == MODE_STATION) {
            WiFi.disconnect();
        } else if (currentMode == MODE_AP) {
            WiFi.softAPdisconnect();
        }
        
        WiFi.mode(WIFI_OFF);
        currentMode = MODE_NONE;
    }

private:
    static WiFiMode currentMode;
    static String macSuffix;
    
    // Connection state tracking (for event handler)
    static volatile ConnectionResult connectionResult;
    static volatile int handshakeFailCount;
    static volatile bool connectionDone;
    static volatile bool eventHandlerRegistered;
    
    // Reset connection state before new attempt
    static void resetConnectionState() {
        connectionResult = CONN_TIMEOUT;  // Default to timeout if nothing else detected
        handshakeFailCount = 0;
        connectionDone = false;
        LOG_INFO("Connection state reset");
    }
    
    // Register WiFi event handler (once)
    static void registerWiFiEventHandler() {
        if (eventHandlerRegistered) return;
        
        WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
            switch (event) {
                case ARDUINO_EVENT_WIFI_STA_CONNECTED:
                    LOG_INFO("WiFi: Connected to AP, waiting for IP...");
                    break;
                    
                case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                    LOG_INFO("WiFi: Got IP address!");
                    connectionResult = CONN_SUCCESS;
                    connectionDone = true;
                    break;
                    
                case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
                    uint8_t reason = info.wifi_sta_disconnected.reason;
                    LOG_PRINTF("WARN ", "WiFi: Disconnected, reason: %d", reason);
                    
                    // Handle specific disconnect reasons
                    switch (reason) {
                        // Wrong password - 4-way handshake timeout
                        case 15:  // WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT
                        case 204: // WIFI_REASON_HANDSHAKE_TIMEOUT
                            handshakeFailCount++;
                            LOG_PRINTF("WARN ", "Handshake timeout count: %d", handshakeFailCount);
                            
                            // After 2 handshake failures, it's definitely wrong password
                            if (handshakeFailCount >= 2) {
                                LOG_ERROR("Wrong password detected!");
                                connectionResult = CONN_WRONG_PASSWORD;
                                connectionDone = true;
                            }
                            break;
                            
                        // Network not found
                        case 201: // WIFI_REASON_NO_AP_FOUND
                            LOG_ERROR("Network not found!");
                            connectionResult = CONN_SSID_NOT_FOUND;
                            connectionDone = true;
                            break;
                            
                        // Authentication failed
                        case 202: // WIFI_REASON_AUTH_FAIL
                        case 2:   // WIFI_REASON_AUTH_EXPIRE
                            LOG_ERROR("Authentication failed!");
                            connectionResult = CONN_AUTH_FAILED;
                            connectionDone = true;
                            break;
                            
                        // Other reasons - let timeout handle it or wait for retry
                        default:
                            LOG_PRINTF("WARN ", "Unhandled disconnect reason: %d", reason);
                            break;
                    }
                    break;
                }
                
                default:
                    break;
            }
        });
        
        eventHandlerRegistered = true;
        LOG_INFO("WiFi event handler registered");
    }
};

// Static member initialization
WiFiManager::WiFiMode WiFiManager::currentMode = WiFiManager::MODE_NONE;
String WiFiManager::macSuffix = "";

// Connection state tracking
volatile WiFiManager::ConnectionResult WiFiManager::connectionResult = WiFiManager::CONN_TIMEOUT;
volatile int WiFiManager::handshakeFailCount = 0;
volatile bool WiFiManager::connectionDone = false;
volatile bool WiFiManager::eventHandlerRegistered = false;

#endif // WIFI_MANAGER_H
