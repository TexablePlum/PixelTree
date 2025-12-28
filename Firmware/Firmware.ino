// ============================================================================
// PixelTree - ARGB Christmas Lights Controller
// XIAO ESP32-S3 Plus Firmware
// ============================================================================
// Features:
// - Dual WiFi modes: Access Point & Station
// - BLE provisioning with ECDH encryption
// - Factory reset via GPIO button
// - Persistent credential storage
// - Robust serial logging
// ============================================================================

#include "Config.h"
#include "SerialLogger.h"
#include "NVSManager.h"
#include "WiFiManager.h"
#include "BLEProvisioning.h"
#include "HTTPProvisioning.h"
#include "LEDController.h"
#include "LEDApi.h"

// ============================================================================
// Global Variables
// ============================================================================

// Reset button state
unsigned long resetButtonPressStart = 0;
bool resetButtonPressed = false;
bool factoryResetTriggered = false;

// ============================================================================
// Setup Function
// ============================================================================

void setup() {
    // Initialize serial logging (first thing!)
    SerialLogger::begin();
    
    LOG_SEPARATOR();
    LOG_SECTION("PixelTree Initialization");
    LOG_SEPARATOR();
    
    // Print system info
    printSystemInfo();
    
    // Initialize GPIO pins
    initGPIO();
    
    // Initialize NVS Manager
    if (!NVSManager::begin()) {
        LOG_ERROR("NVS initialization failed - halting!");
        blinkError();
        while (1) { delay(100); }
    }
    
    // Load and set saved effect immediately (before WiFi connection)
    // This ensures smooth transition from startup animation
    uint8_t savedEffect = NVSManager::loadEffect();
    bool hasStoredEffect = (savedEffect != 0xFF && savedEffect < LEDController::getNumEffects());
    if (hasStoredEffect) {
        LEDController::setEffect(savedEffect);
        LOG_PRINTF("INFO ", "Restored saved effect: %d", savedEffect);
        
        // Also restore saved parameters for this effect
        String savedParams = NVSManager::loadParams();
        if (!savedParams.isEmpty()) {
            LEDController::loadParamsFromJson(savedParams);
        }
    }
    
    // Load saved brightness
    uint8_t savedBrightness = NVSManager::loadBrightness();
    if (savedBrightness != 0xFF) {
        LEDController::setBrightness(savedBrightness);
        LOG_PRINTF("INFO ", "Restored saved brightness: %d", savedBrightness);
    }
    
    // Note: if no stored effect, effectReady stays false until provisioning sets Rainbow Wave
    // or normal mode sets a default - this is handled below
    
    // Initialize WiFi Manager
    WiFiManager::begin();
    
    // Register AP client event handler for dynamic BLE control
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
            // Client connected to AP
            LOG_INFO("WiFi AP: Client connected");
            
            uint8_t clientCount = WiFi.softAPgetStationNum();
            LOG_PRINTF("INFO", "  Total clients: %d", clientCount);
            
            // First client → stop BLE (unless provisioning completed)
            if (clientCount == 1) {
                if (!BLEProvisioning::isProvisioningCompleted()) {
                    LOG_INFO("Stopping BLE - client using AP");
                    BLEProvisioning::stopAdvertising();
                }
            }
        } 
        else if (event == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) {
            // Client disconnected from AP
            LOG_INFO("WiFi AP: Client disconnected");
            
            // Wait for count to update
            delay(100);
            uint8_t remainingClients = WiFi.softAPgetStationNum();
            LOG_PRINTF("INFO", "  Remaining clients: %d", remainingClients);
            
            // Last client → restart BLE (unless provisioning completed)
            if (remainingClients == 0) {
                if (!BLEProvisioning::isProvisioningCompleted()) {
                    LOG_INFO("Restarting BLE - AP is empty");
                    BLEProvisioning::startAdvertising();
                }
            }
        }
    });

    
    // Check for stored credentials
    String savedSSID, savedPassword;
    bool hasCredentials = NVSManager::loadCredentials(savedSSID, savedPassword);
    
    if (hasCredentials) {
        // Try to connect to saved WiFi
        LOG_SECTION("Attempting Saved WiFi Connection");
        
        WiFiManager::ConnectionResult result = WiFiManager::connectStation(savedSSID, savedPassword);
        if (result == WiFiManager::CONN_SUCCESS) {
            LOG_INFO("Connected to WiFi successfully!");
            LOG_INFO("Device is online - BLE provisioning not needed");
            
            // Effect was already loaded above, but ensure effectReady is set
            // in case there was no stored effect
            if (!hasStoredEffect) {
                LEDController::setEffect(0);  // Default to Solid
                LOG_INFO("No saved effect, using default: Solid");
            }
            
            // Start HTTP server for LED control in Station mode
            if (HTTPProvisioning::begin()) {
                LEDApi::begin(HTTPProvisioning::getServer());
                LOG_INFO("HTTP server with LED API started in Station mode");
            }
            
            // Optional: Start BLE anyway for additional control
            // BLEProvisioning::begin();
        } else {
            LOG_WARN("Failed to connect to saved WiFi");
            LOG_INFO("Starting provisioning mode...");
            startProvisioningMode();
        }
    } else {
        // No credentials - start provisioning
        LOG_INFO("No saved credentials found");
        LOG_INFO("Starting provisioning mode...");
        startProvisioningMode();
    }
    
    LOG_SEPARATOR();
    LOG_SECTION("Setup Complete");
    printCurrentStatus();
    LOG_SEPARATOR();
    
    LOG_INFO("Entering main loop...");
}

// ============================================================================
// Loop Function
// ============================================================================

void loop() {
    // Check for factory reset button
    handleResetButton();
    
    // Monitor WiFi connection with smart retry
    static unsigned long lastWiFiCheck = 0;
    static unsigned long lastReconnectAttempt = 0;
    static int reconnectAttempts = 0;
    static const unsigned long BASE_RETRY_INTERVAL = 30000;  // 30s base
    static const unsigned long MAX_RETRY_INTERVAL = 300000;  // 5 min max
    
    if (millis() - lastWiFiCheck > 10000) { // Check every 10s
        lastWiFiCheck = millis();
        
        if (WiFiManager::getMode() == WiFiManager::MODE_STATION) {
            if (!WiFiManager::isConnected()) {
                // Calculate retry interval with exponential backoff (caps at ~5 min)
                unsigned long retryInterval = BASE_RETRY_INTERVAL * (1 << min(reconnectAttempts, 4));
                retryInterval = min(retryInterval, MAX_RETRY_INTERVAL);
                
                // Check if enough time has passed since last attempt
                if (millis() - lastReconnectAttempt > retryInterval || lastReconnectAttempt == 0) {
                    lastReconnectAttempt = millis();
                    
                    String ssid, password;
                    if (NVSManager::loadCredentials(ssid, password)) {
                        reconnectAttempts++;
                        LOG_PRINTF("INFO ", "Reconnect attempt #%d (next retry in %lus)", 
                            reconnectAttempts, retryInterval / 1000);
                        
                        WiFiManager::ConnectionResult result = WiFiManager::connectStation(ssid, password);
                        
                        if (result == WiFiManager::CONN_SUCCESS) {
                            LOG_INFO("Reconnected to WiFi successfully!");
                            reconnectAttempts = 0;  // Reset counter on success
                            lastReconnectAttempt = 0;
                            
                            // Restart mDNS after reconnect
                            MDNS.end();
                            if (MDNS.begin(WiFiManager::getDeviceName().c_str())) {
                                MDNS.addService("http", "tcp", HTTP_SERVER_PORT);
                                MDNS.addServiceTxt("http", "tcp", "macSuffix", WiFiManager::getMacSuffix());
                                MDNS.addServiceTxt("http", "tcp", "deviceName", WiFiManager::getDeviceName());
                                LOG_INFO("mDNS restarted after reconnect");
                            }
                        } else {
                            LOG_PRINTF("WARN ", "Reconnect failed (reason: %d), will keep trying...", result);
                        }
                    }
                }
            } else {
                // Connected - reset counters
                if (reconnectAttempts > 0) {
                    reconnectAttempts = 0;
                    lastReconnectAttempt = 0;
                }
            }
        }
    }
    
    // Blink LED to indicate alive
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 2000) {
        lastBlink = millis();
        blinkLED(1, 50);
    }
    
    delay(10); // Small delay to prevent watchdog issues
}

// ============================================================================
// Helper Functions
// ============================================================================

// Print system information
void printSystemInfo() {
    LOG_INFO("System Information:");
    LOG_INFO("  Chip: ESP32-S3");
    LOG_PRINTF("INFO ", "  CPU Freq: %d MHz", ESP.getCpuFreqMHz());
    LOG_PRINTF("INFO ", "  Flash: %d MB", ESP.getFlashChipSize() / (1024 * 1024));
    LOG_PRINTF("INFO ", "  PSRAM: %d KB", ESP.getPsramSize() / 1024);
    LOG_PRINTF("INFO ", "  Free Heap: %d KB", ESP.getFreeHeap() / 1024);
    
    #if DEV_MODE
    LOG_WARN("DEV_MODE: ENABLED (credentials reset on boot)");
    #else
    LOG_INFO("DEV_MODE: DISABLED (credentials persistent)");
    #endif
}

// Initialize GPIO pins
void initGPIO() {
    LOG_INFO("Initializing GPIO...");
    
    // Reset button (GPIO9) - Active LOW with pull-up
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
    LOG_PRINTF("INFO ", "  Reset Button: GPIO%d (active LOW)", RESET_BUTTON_PIN);
    
    // Built-in LED
    pinMode(LED_BUILTIN_PIN, OUTPUT);
    digitalWrite(LED_BUILTIN_PIN, LOW);
    LOG_PRINTF("INFO ", "  Status LED: GPIO%d", LED_BUILTIN_PIN);
    
    // Initialize LED Controller (runs on Core 0)
    if (LEDController::begin()) {
        LOG_INFO("LED Controller started successfully");
    } else {
        LOG_ERROR("Failed to start LED Controller!");
    }
}

// Start provisioning mode (BLE + AP + HTTP)
void startProvisioningMode() {
    LOG_SECTION("Starting Provisioning Mode");
    
    // Only set Rainbow Wave if no saved effect was loaded
    // This preserves user's effect choice after restart
    if (!LEDController::isEffectReady()) {
        LEDController::setEffect(4);
        LOG_INFO("Provisioning effect set: Rainbow Wave");
    } else {
        LOG_INFO("Using saved effect from NVS");
    }
    
    // Start Access Point
    if (WiFiManager::startAP()) {
        LOG_INFO("Access Point started successfully");
    } else {
        LOG_ERROR("Failed to start Access Point!");
    }
    
    // Start BLE provisioning (Method 1: BLE + WiFi)
    if (BLEProvisioning::begin()) {
        LOG_INFO("BLE Provisioning started successfully");
    } else {
        LOG_ERROR("Failed to start BLE Provisioning!");
    }
    
    // Start HTTP provisioning (Method 2: AP + HTTP)
    if (HTTPProvisioning::begin()) {
        LOG_INFO("HTTP Provisioning started successfully");
        
        // Add LED API routes to the same HTTP server
        LEDApi::begin(HTTPProvisioning::getServer());
        LOG_INFO("LED API routes added to HTTP server");
    } else {
        LOG_ERROR("Failed to start HTTP Provisioning!");
    }
}

// Handle factory reset button
void handleResetButton() {
    bool buttonState = digitalRead(RESET_BUTTON_PIN) == LOW; // Active LOW
    
    if (buttonState && !resetButtonPressed) {
        // Button just pressed
        resetButtonPressed = true;
        resetButtonPressStart = millis();
        LOG_DEBUG("Reset button pressed");
    }
    else if (!buttonState && resetButtonPressed) {
        // Button released
        resetButtonPressed = false;
        unsigned long pressDuration = millis() - resetButtonPressStart;
        LOG_PRINTF("DEBUG", "Reset button released after %lu ms", pressDuration);
    }
    else if (buttonState && resetButtonPressed) {
        // Button still being held
        unsigned long pressDuration = millis() - resetButtonPressStart;
        
        if (pressDuration >= RESET_BUTTON_HOLD_MS && !factoryResetTriggered) {
            // Factory reset triggered!
            factoryResetTriggered = true;
            performFactoryReset();
        }
    }
}

// Perform factory reset
void performFactoryReset() {
    LOG_SEPARATOR();
    LOG_WARN("!!! FACTORY RESET TRIGGERED !!!");
    LOG_SEPARATOR();
    
    // Blink LED rapidly as feedback
    for (int i = 0; i < 10; i++) {
        digitalWrite(LED_BUILTIN_PIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN_PIN, LOW);
        delay(50);
    }
    
    // Clear credentials
    NVSManager::clearCredentials();
    
    LOG_INFO("Factory reset complete - rebooting...");
    delay(1000);
    
    // Reboot
    ESP.restart();
}

// Print current status
void printCurrentStatus() {
    LOG_INFO("Current Status:");
    LOG_PRINTF("INFO ", "  Device Name: %s", WiFiManager::getDeviceName().c_str());
    
    // WiFi status
    if (WiFiManager::getMode() == WiFiManager::MODE_AP) {
        LOG_INFO("  WiFi Mode: Access Point");
        LOG_PRINTF("INFO ", "  AP IP: %s", WiFi.softAPIP().toString().c_str());
    } else if (WiFiManager::getMode() == WiFiManager::MODE_STATION) {
        LOG_INFO("  WiFi Mode: Station");
        LOG_PRINTF("INFO ", "  IP Address: %s", WiFi.localIP().toString().c_str());
        LOG_PRINTF("INFO ", "  RSSI: %d dBm", WiFi.RSSI());
    }
    
    // BLE status
    if (BLEProvisioning::isConnected()) {
        LOG_INFO("  BLE: Connected");
    } else if (BLEProvisioning::getState() != BLEProvisioning::STATE_IDLE) {
        LOG_INFO("  BLE: Advertising");
    }
    
    // Credentials
    if (NVSManager::isProvisioned()) {
        LOG_PRINTF("INFO ", "  Saved Network: %s", NVSManager::getSSID().c_str());
    } else {
        LOG_INFO("  Saved Network: None");
    }
}

// Blink LED
void blinkLED(int times, int delayMs) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_BUILTIN_PIN, HIGH);
        delay(delayMs);
        digitalWrite(LED_BUILTIN_PIN, LOW);
        delay(delayMs);
    }
}

// Blink error pattern (continuous fast blink)
void blinkError() {
    LOG_ERROR("Critical error - entering error state");
    while (1) {
        blinkLED(3, 100);
        delay(1000);
    }
}
