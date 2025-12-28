#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include "Config.h"
#include "SerialLogger.h"

// ============================================================================
// NVSManager - Credential Storage with Dev Mode Reset
// ============================================================================
// Features:
// - Persistent WiFi credential storage
// - DEV_MODE auto-clear on boot
// - Factory reset capability
// - Provisioning state tracking
// ============================================================================

class NVSManager {
public:
    // Initialize NVS and handle dev mode
    static bool begin() {
        LOG_INFO("Initializing NVS Manager...");
        
        // Open preferences in read-write mode
        if (!prefs.begin(NVS_NAMESPACE, false)) {
            LOG_ERROR("Failed to open NVS namespace!");
            return false;
        }
        
        // DEV MODE: Clear credentials on boot
        #if DEV_MODE
        LOG_WARN("DEV_MODE enabled - clearing credentials on boot!");
        clearCredentials();
        #endif
        
        // Check if provisioned
        bool provisioned = prefs.getBool(NVS_KEY_PROVISIONED, false);
        if (provisioned) {
            LOG_INFO("Device previously provisioned");
            logStoredCredentials();
        } else {
            LOG_INFO("Device not provisioned - fresh start");
        }
        
        return true;
    }
    
    // Save WiFi credentials
    static bool saveCredentials(const String& ssid, const String& password) {
        LOG_INFO("Saving WiFi credentials to NVS...");
        
        if (ssid.isEmpty()) {
            LOG_ERROR("Cannot save empty SSID!");
            return false;
        }
        
        // Save SSID
        if (!prefs.putString(NVS_KEY_SSID, ssid)) {
            LOG_ERROR("Failed to save SSID to NVS!");
            return false;
        }
        
        // Save password (can be empty for open networks)
        if (!prefs.putString(NVS_KEY_PASSWORD, password)) {
            LOG_ERROR("Failed to save password to NVS!");
            return false;
        }
        
        // Mark as provisioned
        if (!prefs.putBool(NVS_KEY_PROVISIONED, true)) {
            LOG_ERROR("Failed to set provisioned flag!");
            return false;
        }
        
        LOG_INFO("Credentials saved successfully");
        LOG_PRINTF("INFO ", "  SSID: %s", ssid.c_str());
        LOG_INFO("  Password: ********");
        
        return true;
    }
    
    // Load WiFi credentials
    static bool loadCredentials(String& ssid, String& password) {
        if (!isProvisioned()) {
            LOG_DEBUG("No credentials stored");
            return false;
        }
        
        ssid = prefs.getString(NVS_KEY_SSID, "");
        password = prefs.getString(NVS_KEY_PASSWORD, "");
        
        if (ssid.isEmpty()) {
            LOG_ERROR("Stored SSID is empty!");
            return false;
        }
        
        LOG_INFO("Credentials loaded from NVS");
        LOG_PRINTF("INFO ", "  SSID: %s", ssid.c_str());
        
        return true;
    }
    
    // Check if device is provisioned
    static bool isProvisioned() {
        return prefs.getBool(NVS_KEY_PROVISIONED, false);
    }
    
    // Clear all credentials (factory reset)
    static void clearCredentials() {
        LOG_WARN("Clearing stored credentials...");
        
        prefs.remove(NVS_KEY_SSID);
        prefs.remove(NVS_KEY_PASSWORD);
        prefs.remove(NVS_KEY_PROVISIONED);
        prefs.remove(NVS_KEY_LED_EFFECT);
        prefs.remove("led_bright");
        prefs.remove("led_params");
        
        LOG_INFO("Credentials cleared - device reset to factory state");
    }
    
    // Save LED effect to NVS
    static void saveEffect(uint8_t effectId) {
        prefs.putUChar(NVS_KEY_LED_EFFECT, effectId);
        LOG_PRINTF("DEBUG", "LED effect saved to NVS: %d", effectId);
    }
    
    // Load LED effect from NVS (returns 0xFF if not set)
    static uint8_t loadEffect() {
        uint8_t effect = prefs.getUChar(NVS_KEY_LED_EFFECT, 0xFF);
        if (effect != 0xFF) {
            LOG_PRINTF("INFO ", "LED effect loaded from NVS: %d", effect);
        }
        return effect;
    }
    
    // Save brightness to NVS
    static void saveBrightness(uint8_t brightness) {
        prefs.putUChar("led_bright", brightness);
        LOG_PRINTF("DEBUG", "Brightness saved to NVS: %d", brightness);
    }
    
    // Load brightness from NVS (returns 0xFF if not set)
    static uint8_t loadBrightness() {
        return prefs.getUChar("led_bright", 0xFF);
    }
    
    // Save effect parameters to NVS as JSON string
    static void saveParams(const String& paramsJson) {
        prefs.putString("led_params", paramsJson);
        LOG_DEBUG("Effect params saved to NVS");
    }
    
    // Load effect parameters from NVS
    static String loadParams() {
        return prefs.getString("led_params", "");
    }
    
    // Get stored SSID (for display purposes)
    static String getSSID() {
        return prefs.getString(NVS_KEY_SSID, "");
    }
    
    // Close NVS
    static void end() {
        prefs.end();
    }

private:
    static Preferences prefs;
    
    // Log stored credentials (for debugging)
    static void logStoredCredentials() {
        String ssid = prefs.getString(NVS_KEY_SSID, "");
        if (!ssid.isEmpty()) {
            LOG_PRINTF("INFO ", "  Stored SSID: %s", ssid.c_str());
        }
    }
};

// Static member initialization
Preferences NVSManager::prefs;

#endif // NVS_MANAGER_H
