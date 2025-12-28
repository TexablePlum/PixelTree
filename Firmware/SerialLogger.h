#ifndef SERIAL_LOGGER_H
#define SERIAL_LOGGER_H

#include <Arduino.h>
#include "Config.h"

// ============================================================================
// SerialLogger - Bulletproof Serial Logging for ESP32
// ============================================================================
// Features:
// - Timestamped messages
// - Log level filtering
// - Buffer management to prevent truncation
// - Safe printing with write checks
// - No dynamic allocation in hot path
// ============================================================================

class SerialLogger {
public:
    // Initialize serial communications
    static void begin() {
        Serial.begin(SERIAL_BAUD_RATE);
        
        // Wait for serial with timeout (important for USB CDC)
        unsigned long start = millis();
        while (!Serial && (millis() - start < SERIAL_WAIT_MS)) {
            delay(10);
        }
        
        // Give USB CDC more time to stabilize and enumerate
        delay(500);
        
        Serial.println();
        Serial.println(F("========================================"));
        Serial.println(F("    PixelTree Firmware v1.0.0"));
        Serial.println(F("    XIAO ESP32-S3 ARGB Controller"));
        Serial.println(F("========================================"));
        Serial.flush();
    }
    
    // Log error message (always shown)
    static void error(const char* message) {
        printLog("ERROR", message);
    }
    
    static void error(const String& message) {
        error(message.c_str());
    }
    
    // Log warning message
    static void warn(const char* message) {
        if (LOG_LEVEL_WARN <= LOG_LEVEL_DEBUG) {
            printLog("WARN ", message);
        }
    }
    
    static void warn(const String& message) {
        warn(message.c_str());
    }
    
    // Log info message
    static void info(const char* message) {
        if (LOG_LEVEL_INFO <= LOG_LEVEL_DEBUG) {
            printLog("INFO ", message);
        }
    }
    
    static void info(const String& message) {
        info(message.c_str());
    }
    
    // Log debug message (only if enabled)
    static void debug(const char* message) {
        #if LOG_ENABLE_DEBUG
        if (LOG_LEVEL_DEBUG <= LOG_LEVEL_DEBUG) {
            printLog("DEBUG", message);
        }
        #endif
    }
    
    static void debug(const String& message) {
        debug(message.c_str());
    }
    
    // Print formatted message (sprintf-style)
    static void printf(const char* level, const char* format, ...) {
        char buffer[LOG_BUFFER_SIZE];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, LOG_BUFFER_SIZE, format, args);
        va_end(args);
        
        printLog(level, buffer);
    }
    
    // Force flush output
    static void flush() {
        Serial.flush();
    }
    
    // Print separator line
    static void separator() {
        Serial.println(F("----------------------------------------"));
        Serial.flush();
    }
    
    // Print section header
    static void section(const char* title) {
        Serial.println();
        Serial.print(F("=== "));
        Serial.print(title);
        Serial.println(F(" ==="));
        Serial.flush();
    }

private:
    // Core logging function with timestamp and level
    static void printLog(const char* level, const char* message) {
        // Wait for serial buffer availability
        waitForBuffer();
        
        // Print timestamp if enabled
        #if LOG_ENABLE_TIMESTAMPS
        char timestamp[16];
        snprintf(timestamp, sizeof(timestamp), "[%10lu] ", millis());
        Serial.print(timestamp);
        #endif
        
        // Print level
        Serial.print("[");
        Serial.print(level);
        Serial.print("] ");
        
        // Print message in chunks to avoid buffer overflow
        printSafe(message);
        
        Serial.println();
        
        // Always flush to ensure message is sent before next log
        Serial.flush();
    }
    
    // Wait for serial buffer to have space
    static void waitForBuffer() {
        unsigned long start = millis();
        // Wait for more space (128 bytes instead of 64) with longer timeout
        while (Serial.availableForWrite() < 128 && (millis() - start < 200)) {
            delay(2);  
        }
    }
    
    // Print string safely in chunks
    static void printSafe(const char* message) {
        size_t len = strlen(message);
        size_t pos = 0;
        const size_t chunkSize = 32; 
        
        while (pos < len) {
            // Wait for buffer space
            waitForBuffer();
            
            // Calculate chunk size
            size_t remaining = len - pos;
            size_t toWrite = (remaining > chunkSize) ? chunkSize : remaining;
            
            // Write chunk
            Serial.write((const uint8_t*)(message + pos), toWrite);
            pos += toWrite;
            
            // Small delay between chunks to prevent overflow
            if (pos < len) {
                delayMicroseconds(100);
            }
        }
    }
};

// ============================================================================
// Convenience Macros
// ============================================================================
#define LOG_ERROR(msg)   SerialLogger::error(msg)
#define LOG_WARN(msg)    SerialLogger::warn(msg)
#define LOG_INFO(msg)    SerialLogger::info(msg)
#define LOG_DEBUG(msg)   SerialLogger::debug(msg)
#define LOG_PRINTF(lvl, fmt, ...) SerialLogger::printf(lvl, fmt, ##__VA_ARGS__)
#define LOG_SECTION(title) SerialLogger::section(title)
#define LOG_SEPARATOR()   SerialLogger::separator()

#endif // SERIAL_LOGGER_H
