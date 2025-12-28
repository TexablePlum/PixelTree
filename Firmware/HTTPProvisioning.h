#ifndef HTTP_PROVISIONING_H
#define HTTP_PROVISIONING_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "SerialLogger.h"
#include "WiFiManager.h"
#include "NVSManager.h"

// ============================================================================
// HTTPProvisioning - WiFi Provisioning via HTTP REST API
// ============================================================================
// Features:
// - REST API for WiFi network scanning
// - Credential submission endpoint
// - CORS support for mobile apps
// - JSON responses
// ============================================================================

class HTTPProvisioning {
public:
    // Provisioning state
    enum ProvisioningState {
        STATE_IDLE,
        STATE_SCANNING,
        STATE_SCANNED,
        STATE_CONNECTING,
        STATE_SUCCESS,
        STATE_FAILED
    };
    
    // Initialize HTTP server
    static bool begin() {
        LOG_SECTION("Initializing HTTP Provisioning");
        
        if (server != nullptr) {
            LOG_WARN("HTTP server already running");
            return true;
        }
        
        server = new AsyncWebServer(HTTP_SERVER_PORT);
        
        // Setup routes
        setupRoutes();
        
        // Start server
        server->begin();
        
        currentState = STATE_IDLE;
        LOG_PRINTF("INFO ", "HTTP server started on port %d", HTTP_SERVER_PORT);
        LOG_INFO("HTTP Provisioning ready!");
        
        return true;
    }
    
    // Stop HTTP server
    static void stop() {
        if (server != nullptr) {
            server->end();
            delete server;
            server = nullptr;
            LOG_INFO("HTTP server stopped");
        }
    }
    
    // Get current state
    static ProvisioningState getState() {
        return currentState;
    }
    
    // Get server pointer for adding additional routes (e.g., LED API)
    static AsyncWebServer* getServer() {
        return server;
    }

private:
    static AsyncWebServer* server;
    static ProvisioningState currentState;
    static String lastScanResults;
    static String receivedSSID;
    static String receivedPassword;
    
    // ========================================================================
    // Route Setup
    // ========================================================================
    
    static void setupRoutes() {
        LOG_INFO("Setting up HTTP routes...");
        
        // CORS preflight for all routes
        server->on("/api/*", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse(200);
            addCorsHeaders(response);
            request->send(response);
        });
        
        // GET /api/status - Get provisioning status
        server->on("/api/status", HTTP_GET, handleStatus);
        
        // POST /api/scan - Trigger WiFi scan
        server->on("/api/scan", HTTP_POST, handleScan);
        
        // GET /api/networks - Get scan results
        server->on("/api/networks", HTTP_GET, handleNetworks);
        
        // POST /api/provision - Submit credentials
        AsyncCallbackJsonWebHandler* provisionHandler = new AsyncCallbackJsonWebHandler(
            "/api/provision",
            handleProvision
        );
        server->addHandler(provisionHandler);
        
        // Catch-all 404
        server->onNotFound([](AsyncWebServerRequest *request) {
            LOG_PRINTF("WARN ", "404 Not Found: %s", request->url().c_str());
            AsyncWebServerResponse *response = request->beginResponse(
                404, 
                "application/json", 
                "{\"error\":\"Not Found\"}"
            );
            addCorsHeaders(response);
            request->send(response);
        });
        
        LOG_INFO("HTTP routes configured");
    }
    
    // ========================================================================
    // Route Handlers
    // ========================================================================
    
    // GET /api/status
    static void handleStatus(AsyncWebServerRequest *request) {
        LOG_INFO("GET /api/status");
        
        StaticJsonDocument<256> doc;
        doc["state"] = stateToString(currentState);
        doc["connected"] = WiFiManager::isConnected();
        doc["deviceName"] = WiFiManager::getDeviceName();
        doc["macSuffix"] = WiFiManager::getMacSuffix();
        
        if (WiFiManager::isConnected()) {
            doc["ip"] = WiFi.localIP().toString();
            doc["ssid"] = WiFi.SSID();
        }
        
        if (NVSManager::isProvisioned()) {
            doc["provisioned"] = true;
            doc["saved_ssid"] = NVSManager::getSSID();
        } else {
            doc["provisioned"] = false;
        }
        
        String response;
        serializeJson(doc, response);
        
        AsyncWebServerResponse *res = request->beginResponse(200, "application/json", response);
        addCorsHeaders(res);
        request->send(res);
    }
    
    // POST /api/scan
    static void handleScan(AsyncWebServerRequest *request) {
        LOG_INFO("POST /api/scan");
        
        currentState = STATE_SCANNING;
        
        // Trigger WiFi scan (blocks for ~2-3 seconds)
        lastScanResults = WiFiManager::scanNetworks();
        
        currentState = STATE_SCANNED;
        
        StaticJsonDocument<128> doc;
        doc["status"] = "scanning_complete";
        doc["count"] = countNetworks(lastScanResults);
        
        String response;
        serializeJson(doc, response);
        
        AsyncWebServerResponse *res = request->beginResponse(200, "application/json", response);
        addCorsHeaders(res);
        request->send(res);
    }
    
    // GET /api/networks
    static void handleNetworks(AsyncWebServerRequest *request) {
        LOG_INFO("GET /api/networks");
        
        if (lastScanResults.isEmpty()) {
            // No scan results yet
            AsyncWebServerResponse *res = request->beginResponse(
                400,
                "application/json",
                "{\"error\":\"No scan results. Call POST /api/scan first\"}"
            );
            addCorsHeaders(res);
            request->send(res);
            return;
        }
        
        AsyncWebServerResponse *res = request->beginResponse(
            200,
            "application/json",
            lastScanResults
        );
        addCorsHeaders(res);
        request->send(res);
    }
    
    // POST /api/provision
    static void handleProvision(AsyncWebServerRequest *request, JsonVariant &json) {
        LOG_INFO("POST /api/provision");
        
        JsonObject jsonObj = json.as<JsonObject>();
        
        if (!jsonObj.containsKey("ssid") || !jsonObj.containsKey("password")) {
            AsyncWebServerResponse *res = request->beginResponse(
                400,
                "application/json",
                "{\"error\":\"Missing ssid or password\"}"
            );
            addCorsHeaders(res);
            request->send(res);
            return;
        }
        
        receivedSSID = jsonObj["ssid"].as<String>();
        receivedPassword = jsonObj["password"].as<String>();
        
        LOG_PRINTF("INFO ", "Received credentials for SSID: %s", receivedSSID.c_str());
        
        currentState = STATE_CONNECTING;
        
        // Attempt WiFi connection
        bool success = WiFiManager::connectStation(receivedSSID, receivedPassword);
        
        if (success) {
            // Save credentials
            NVSManager::saveCredentials(receivedSSID, receivedPassword);
            currentState = STATE_SUCCESS;
            
            LOG_INFO("Provisioning successful!");
            
            StaticJsonDocument<256> doc;
            doc["status"] = "success";
            doc["connected"] = true;
            doc["ip"] = WiFi.localIP().toString();
            
            String response;
            serializeJson(doc, response);
            
            AsyncWebServerResponse *res = request->beginResponse(200, "application/json", response);
            addCorsHeaders(res);
            request->send(res);
        } else {
            currentState = STATE_FAILED;
            
            LOG_ERROR("WiFi connection failed!");
            
            AsyncWebServerResponse *res = request->beginResponse(
                400,
                "application/json",
                "{\"status\":\"failed\",\"error\":\"Could not connect to WiFi\"}"
            );
            addCorsHeaders(res);
            request->send(res);
        }
    }
    
    // ========================================================================
    // Helper Functions
    // ========================================================================
    
    // Add CORS headers to response
    static void addCorsHeaders(AsyncWebServerResponse *response) {
        response->addHeader("Access-Control-Allow-Origin", HTTP_CORS_ORIGIN);
        response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    }
    
    // Convert state enum to string
    static const char* stateToString(ProvisioningState state) {
        switch (state) {
            case STATE_IDLE: return "idle";
            case STATE_SCANNING: return "scanning";
            case STATE_SCANNED: return "scanned";
            case STATE_CONNECTING: return "connecting";
            case STATE_SUCCESS: return "success";
            case STATE_FAILED: return "failed";
            default: return "unknown";
        }
    }
    
    // Count networks in JSON array
    static int countNetworks(const String& json) {
        StaticJsonDocument<2048> doc;
        DeserializationError error = deserializeJson(doc, json);
        if (error) return 0;
        return doc.as<JsonArray>().size();
    }
};

// Static member initialization
AsyncWebServer* HTTPProvisioning::server = nullptr;
HTTPProvisioning::ProvisioningState HTTPProvisioning::currentState = HTTPProvisioning::STATE_IDLE;
String HTTPProvisioning::lastScanResults = "";
String HTTPProvisioning::receivedSSID = "";
String HTTPProvisioning::receivedPassword = "";

#endif // HTTP_PROVISIONING_H
