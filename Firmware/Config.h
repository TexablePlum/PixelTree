#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// PixelTree Configuration
// ESP32-S3 ARGB Christmas Lights Controller
// ============================================================================

// ----------------------------------------------------------------------------
// Serial Configuration
// ----------------------------------------------------------------------------
#define SERIAL_BAUD_RATE      921600    // High-speed for robust logging
#define SERIAL_WAIT_MS        1000      // Max wait for Serial on boot

// ----------------------------------------------------------------------------
// Device Naming (MAC-based suffix added dynamically)
// ----------------------------------------------------------------------------
#define DEVICE_NAME_PREFIX    "PixelTree"  // Will become "PixelTree-A1B2"
#define AP_SSID_PREFIX        "PixelTree"  // Will become "PixelTree-A1B2"

// ----------------------------------------------------------------------------
// WiFi Configuration
// ----------------------------------------------------------------------------
#define WIFI_CONNECT_TIMEOUT_MS   30000  // 30s to connect to WiFi 
#define WIFI_RECONNECT_INTERVAL   5000   // 5s between retry attempts
#define AP_CHANNEL                1      // WiFi channel for AP mode
#define AP_MAX_CONNECTIONS        4      // Max clients in AP mode
#define AP_HIDDEN                 false  // AP visibility

// ----------------------------------------------------------------------------
// BLE Configuration
// ----------------------------------------------------------------------------
#define BLE_DEVICE_NAME_PREFIX    "PixelTree"  // Will become "PixelTree-A1B2"
#define BLE_MTU_SIZE              512          // Maximum transmission unit

// BLE UUIDs - Custom services for PixelTree
#define BLE_SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

// WiFi Scan Service
#define WIFI_SCAN_SERVICE_UUID    "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define WIFI_SCAN_TRIGGER_UUID    "6e400002-b5a3-f393-e0a9-e50e24dcca9e"  // Write to trigger
#define WIFI_SCAN_RESULTS_UUID    "6e400003-b5a3-f393-e0a9-e50e24dcca9e"  // Read results

// Credential Service
#define CREDENTIAL_SERVICE_UUID   "7e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CREDENTIAL_SSID_UUID      "7e400002-b5a3-f393-e0a9-e50e24dcca9e"  // Write SSID
#define CREDENTIAL_PASS_UUID      "7e400003-b5a3-f393-e0a9-e50e24dcca9e"  // Write encrypted password
#define CREDENTIAL_CONNECT_UUID   "7e400004-b5a3-f393-e0a9-e50e24dcca9e"  // Write to connect
#define CREDENTIAL_STATUS_UUID    "7e400005-b5a3-f393-e0a9-e50e24dcca9e"  // Notify status

// Key Exchange Service (ECDH)
#define KEY_EXCHANGE_SERVICE_UUID "8e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define PUBLIC_KEY_ESP_UUID       "8e400002-b5a3-f393-e0a9-e50e24dcca9e"  // Read ESP public key
#define PUBLIC_KEY_APP_UUID       "8e400003-b5a3-f393-e0a9-e50e24dcca9e"  // Write app public key

// ----------------------------------------------------------------------------
// NVS (Non-Volatile Storage) Configuration
// ----------------------------------------------------------------------------
#define NVS_NAMESPACE             "pixeltree"
#define NVS_KEY_SSID              "wifi_ssid"
#define NVS_KEY_PASSWORD          "wifi_pass"
#define NVS_KEY_PROVISIONED       "provisioned"
#define NVS_KEY_LED_EFFECT        "led_effect"

// ----------------------------------------------------------------------------
// GPIO Pin Configuration
// ----------------------------------------------------------------------------
#define RESET_BUTTON_PIN          9      // GPIO9 for factory reset button
#define RESET_BUTTON_HOLD_MS      5000   // 5 seconds to trigger reset
#define LED_BUILTIN_PIN           21     // Built-in LED on XIAO ESP32S3

// Future ARGB LED pins
#define ARGB_DATA_PIN             44     // GPIO44 = D7 on XIAO ESP32S3
#define ARGB_NUM_LEDS             75     // 75 ARGB LEDs on the chain
#define LED_TARGET_FPS            60     // Target frame rate for animations

// ----------------------------------------------------------------------------
// Development Mode
// ----------------------------------------------------------------------------
#define DEV_MODE                  false   // Reset credentials on boot (development)

// ----------------------------------------------------------------------------
// Task Configuration (FreeRTOS)
// ----------------------------------------------------------------------------
#define TASK_STACK_SIZE_WIFI      4096
#define TASK_STACK_SIZE_BLE       4096
#define TASK_STACK_SIZE_LOGGER    2048
#define TASK_PRIORITY_WIFI        2
#define TASK_PRIORITY_BLE         1
#define TASK_PRIORITY_LOGGER      0
#define TASK_STACK_SIZE_LED       8192
#define TASK_PRIORITY_LED         3      // Higher than WiFi/BLE for smooth animations

// ----------------------------------------------------------------------------
// Logging Configuration
// ----------------------------------------------------------------------------
#define LOG_BUFFER_SIZE           256    // Max single log message size
#define LOG_ENABLE_TIMESTAMPS     true
#define LOG_ENABLE_DEBUG          true   // Enable debug level logs

// Log levels
#define LOG_LEVEL_ERROR           0
#define LOG_LEVEL_WARN            1
#define LOG_LEVEL_INFO            2
#define LOG_LEVEL_DEBUG           3

// ----------------------------------------------------------------------------
// Security Configuration
// ----------------------------------------------------------------------------
#define SECURITY_AES_KEY_SIZE     16     // AES-128 (16 bytes)
#define SECURITY_ECDH_KEY_SIZE    32     // P-256 curve (32 bytes)

// ----------------------------------------------------------------------------
// HTTP Server Configuration
// ----------------------------------------------------------------------------
#define HTTP_SERVER_PORT          80     // HTTP server port for AP mode
#define HTTP_CORS_ORIGIN          "*"    // CORS allow origin (all for now)

// ----------------------------------------------------------------------------
// Utility Macros
// ----------------------------------------------------------------------------
#define ARRAY_SIZE(x)             (sizeof(x) / sizeof((x)[0]))
#define MAC_ADDR_SIZE             6
#define MAC_SUFFIX_LEN            4      // Last 4 hex chars of MAC

#endif // CONFIG_H
