/*
 * LEDController.h - LED Animation Controller with FreeRTOS Task
 * 
 * Manages ARGB LED effects running in parallel with WiFi/BLE
 */

#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include <FastLED.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "SerialLogger.h"

// Include effect definitions (must come before Effects.h)
#include "EffectDefs.h"
#include "Effects.h"

// ============================================================================
// LEDController - FreeRTOS Task for LED Animations
// ============================================================================
// Features:
// - Runs on Core 0 (separate from WiFi on Core 1)
// - Non-blocking effect rendering at ~60 FPS
// - Live parameter updates via setParam()
// ============================================================================

class LEDController {
public:
    // Effect definition
    struct EffectEntry {
        const char* name;
        void (*func)();
        uint8_t category;
    };

    // Initialize LED controller and start FreeRTOS task
    static bool begin() {
        LOG_SECTION("Initializing LED Controller");
        
        // Initialize FastLED
        FastLED.addLeds<WS2812, ARGB_DATA_PIN, GRB>(leds, ARGB_NUM_LEDS)
               .setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(brightness);
        FastLED.setMaxPowerInMilliWatts(45000); // 45W max
        
        // Clear LEDs
        FastLED.clear();
        FastLED.show();
        
        // Init random seed
        random16_set_seed(esp_random());
        
        LOG_PRINTF("INFO ", "LED Data Pin: GPIO%d", ARGB_DATA_PIN);
        LOG_PRINTF("INFO ", "Number of LEDs: %d", ARGB_NUM_LEDS);
        
        // Play startup animation (blocking - before FreeRTOS task starts)
        playStartupAnimation();
        
        // Create LED task on Core 0
        BaseType_t result = xTaskCreatePinnedToCore(
            ledTask,              // Task function
            "LEDTask",            // Task name
            TASK_STACK_SIZE_LED,  // Stack size
            NULL,                 // Parameters
            TASK_PRIORITY_LED,    // Priority
            &ledTaskHandle,       // Task handle
            0                     // Core 0 (WiFi uses Core 1)
        );
        
        if (result != pdPASS) {
            LOG_ERROR("Failed to create LED task!");
            return false;
        }
        
        LOG_INFO("LED Controller initialized successfully!");
        LOG_PRINTF("INFO ", "Loaded %d effects", NUM_EFFECTS);
        
        return true;
    }
    
    // ========================================================================
    // Control Methods
    // ========================================================================
    
    // Check if an effect was already loaded/set
    static bool isEffectReady() {
        return effectReady;
    }
    
    static void setEffect(uint8_t id) {
        if (id < NUM_EFFECTS) {
            currentEffect = id;
            effectChanged = true;
            effectReady = true;  // Effect is now set, task can proceed
            LOG_PRINTF("INFO ", "Effect changed to: %s", effects[id].name);
        }
    }
    
    static void setPower(bool on) {
        powerOn = on;
        if (!on) {
            FastLED.clear();
            FastLED.show();
        }
        LOG_PRINTF("INFO ", "LED Power: %s", on ? "ON" : "OFF");
    }
    
    static void setBrightness(uint8_t b) {
        brightness = b;
        FastLED.setBrightness(brightness);
        LOG_PRINTF("INFO ", "LED Brightness: %d", brightness);
    }
    
    // Play startup "build" animation - LEDs light up one by one, then crossfade to effect
    static void playStartupAnimation() {
        LOG_INFO("Playing startup animation...");
        
        // Clear all LEDs first
        FastLED.clear();
        FastLED.show();
        
        // Calculate delay per LED (aim for ~2 second total animation)
        uint16_t delayPerLed = max(5, min(30, 2000 / ARGB_NUM_LEDS));
        
        // Build animation: light up each LED sequentially with rainbow
        for (uint16_t i = 0; i < ARGB_NUM_LEDS; i++) {
            uint8_t hue = (i * 256 / 15) & 0xFF;  // Use default size=15
            leds[i] = CHSV(hue, 255, brightness);  // Use current brightness
            FastLED.show();
            delay(delayPerLed);
        }
        
        LOG_INFO("Startup animation complete - ready for effect");
        // Don't set any effect here - let setup() determine the right one
        // effectChanged stays true so first effect frame will render properly
    }
    
    // Load parameters from JSON string (used for NVS restore)
    static void loadParamsFromJson(const String& jsonStr) {
        if (jsonStr.isEmpty()) return;
        
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, jsonStr);
        
        if (error) {
            LOG_PRINTF("WARN ", "Failed to parse params JSON: %s", error.c_str());
            return;
        }
        
        JsonObject params = doc.as<JsonObject>();
        for (JsonPair kv : params) {
            setParam(kv.key().c_str(), kv.value());
        }
        
        LOG_INFO("Effect parameters restored from NVS");
    }
    
    // Set parameter from JSON key-value
    static void setParam(const String& key, JsonVariant value) {
        // Speed parameter
        if (key == "speed" && value.is<uint8_t>()) {
            applySpeedParam(value.as<uint8_t>());
        }
        // Generic color parameter
        else if (key == "color" && value.is<const char*>()) {
            CRGB color = parseColor(value.as<const char*>());
            applyColorParam(color);
        }
        // Intensity parameter
        else if (key == "intensity" && value.is<uint8_t>()) {
            applyIntensityParam(value.as<uint8_t>());
        }
        // Gradient colors
        else if (key == "colorStart" && value.is<const char*>()) {
            gradientParams.colorStart = parseColor(value.as<const char*>());
        }
        else if (key == "colorMiddle" && value.is<const char*>()) {
            gradientParams.colorMiddle = parseColor(value.as<const char*>());
        }
        else if (key == "colorEnd" && value.is<const char*>()) {
            gradientParams.colorEnd = parseColor(value.as<const char*>());
        }
        else if (key == "threePoint" && value.is<bool>()) {
            gradientParams.threePoint = value.as<bool>();
        }
        else if (key == "style" && value.is<uint8_t>()) {
            if (currentEffect == 1) gradientParams.style = (GradientStyle)value.as<uint8_t>();
            else if (currentEffect == 40) policeLightsParams.style = (PoliceStyle)constrain(value.as<uint8_t>(), 0, 2);
        }
        // Spots parameters
        else if (key == "spread" && value.is<uint8_t>()) {
            spotsParams.spread = value.as<uint8_t>();
        }
        else if (key == "width" && value.is<uint8_t>()) {
            spotsParams.width = value.as<uint8_t>();
        }
        else if (key == "fade" && value.is<bool>()) {
            spotsParams.fade = value.as<bool>();
        }
        // Pattern parameters
        else if (key == "colorFg" && value.is<const char*>()) {
            patternParams.colorFg = parseColor(value.as<const char*>());
        }
        else if (key == "colorBg" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 3) patternParams.colorBg = c;  // Pattern effect
            else if (currentEffect == 15) sparkleParams.colorBg = c;  // Sparkle
            else if (currentEffect == 16) glitterParams.bgColor = c;  // Glitter
        }
        else if (key == "fgSize" && value.is<uint8_t>()) {
            patternParams.fgSize = value.as<uint8_t>();
        }
        else if (key == "bgSize" && value.is<uint8_t>()) {
            patternParams.bgSize = value.as<uint8_t>();
        }
        // Color Wave, Scanner, and Running Lights parameters (color1-8)
        else if (key == "color1" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 5) colorWaveParams.colors[0] = c;
            else if (currentEffect == 9) scannerParams.colors[0] = c;
            else if (currentEffect == 11) runningLightsParams.colors[0] = c;
            else if (currentEffect == 26) christmasChaseParams.color1 = c;
            else if (currentEffect == 40) policeLightsParams.color1 = c;
            else if (currentEffect == 39) fadeParams.colors[0] = c;
        }
        else if (key == "color2" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 5) colorWaveParams.colors[1] = c;
            else if (currentEffect == 9) scannerParams.colors[1] = c;
            else if (currentEffect == 11) runningLightsParams.colors[1] = c;
            else if (currentEffect == 26) christmasChaseParams.color2 = c;
            else if (currentEffect == 40) policeLightsParams.color2 = c;
            else if (currentEffect == 39) fadeParams.colors[1] = c;
        }
        else if (key == "color3" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 5) colorWaveParams.colors[2] = c;
            else if (currentEffect == 9) scannerParams.colors[2] = c;
            else if (currentEffect == 11) runningLightsParams.colors[2] = c;
            else if (currentEffect == 39) fadeParams.colors[2] = c;
        }
        else if (key == "color4" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 5) colorWaveParams.colors[3] = c;
            else if (currentEffect == 9) scannerParams.colors[3] = c;
            else if (currentEffect == 11) runningLightsParams.colors[3] = c;
            else if (currentEffect == 39) fadeParams.colors[3] = c;
        }
        else if (key == "color5" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 5) colorWaveParams.colors[4] = c;
            else if (currentEffect == 9) scannerParams.colors[4] = c;
            else if (currentEffect == 39) fadeParams.colors[4] = c;
        }
        else if (key == "color6" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 5) colorWaveParams.colors[5] = c;
            else if (currentEffect == 9) scannerParams.colors[5] = c;
            else if (currentEffect == 39) fadeParams.colors[5] = c;
        }
        else if (key == "color7" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 5) colorWaveParams.colors[6] = c;
            else if (currentEffect == 9) scannerParams.colors[6] = c;
            else if (currentEffect == 39) fadeParams.colors[6] = c;
        }
        else if (key == "color8" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 5) colorWaveParams.colors[7] = c;
            else if (currentEffect == 9) scannerParams.colors[7] = c;
            else if (currentEffect == 39) fadeParams.colors[7] = c;
        }
        else if (key == "direction" && value.is<uint8_t>()) {
            Direction dir = (Direction)value.as<uint8_t>();
            if (currentEffect == 5) colorWaveParams.direction = dir;
            else if (currentEffect == 10) cometParams.direction = dir;
            else if (currentEffect == 29) snowSparkleParams.direction = dir;
        }
        // Rainbow wave size
        else if (key == "size" && value.is<uint8_t>()) {
            rainbowWaveParams.size = value.as<uint8_t>();
        }
        else if (key == "saturation" && value.is<uint8_t>()) {
            rainbowWaveParams.saturation = value.as<uint8_t>();
        }
        // Wavy effect parameters
        else if (key == "amplitude" && value.is<uint8_t>()) {
            wavyParams.amplitude = value.as<uint8_t>();
        }
        else if (key == "frequency" && value.is<uint8_t>()) {
            if (currentEffect == 7) wavyParams.frequency = value.as<uint8_t>();
            else if (currentEffect == 34) lightningParams.frequency = value.as<uint8_t>();
            else if (currentEffect == 41) strobeParams.frequency = value.as<uint8_t>();
        }
        // Two-color effects
        else if (key == "colorPrimary" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 6) oscillateParams.colorPrimary = c;
            else if (currentEffect == 12) androidParams.colorPrimary = c;
            else if (currentEffect == 37) breatheParams.colorPrimary = c;
        }
        else if (key == "colorSecondary" && value.is<const char*>()) {
            CRGB c = parseColor(value.as<const char*>());
            if (currentEffect == 6) oscillateParams.colorSecondary = c;
            else if (currentEffect == 12) androidParams.colorSecondary = c;
            else if (currentEffect == 37) breatheParams.colorSecondary = c;
        }
        else if (key == "pointSize" && value.is<uint8_t>()) {
            oscillateParams.pointSize = value.as<uint8_t>();
        }
        // Gap and trail
        else if (key == "gapSize" && value.is<uint8_t>()) {
            theaterChaseParams.gapSize = value.as<uint8_t>();
        }
        else if (key == "trailLength" && value.is<uint8_t>()) {
            if (currentEffect == 9) scannerParams.trailLength = value.as<uint8_t>();
            else if (currentEffect == 10) cometParams.trailLength = value.as<uint8_t>();
            else if (currentEffect == 35) matrixParams.trailLength = constrain(value.as<uint8_t>(), 3, 30);
        }
        else if (key == "sparkleColor" && value.is<const char*>()) {
            cometParams.sparkleColor = parseColor(value.as<const char*>());
        }
        else if (key == "sparkleEnabled" && value.is<bool>()) {
            cometParams.sparkleEnabled = value.as<bool>();
        }
        else if (key == "numDots" && value.is<uint8_t>()) {
            scannerParams.numDots = value.as<uint8_t>();
        }
        // Boolean modes
        else if (key == "rainbowMode" && value.is<bool>()) {
            theaterChaseParams.rainbowMode = value.as<bool>();
        }
        else if (key == "waveWidth" && value.is<uint8_t>()) {
            runningLightsParams.waveWidth = value.as<uint8_t>();
        }
        else if (key == "shape" && value.is<uint8_t>()) {
            runningLightsParams.shape = (WaveShape)value.as<uint8_t>();
        }
        else if (key == "numColors" && value.is<uint8_t>()) {
            if (currentEffect == 5) colorWaveParams.numColors = value.as<uint8_t>();
            else if (currentEffect == 11) runningLightsParams.numColors = value.as<uint8_t>();
            else if (currentEffect == 39) fadeParams.numColors = constrain(value.as<uint8_t>(), 2, 8);
        }
        else if (key == "dualMode" && value.is<bool>()) {
            if (currentEffect == 9) scannerParams.dualMode = value.as<bool>();
            else if (currentEffect == 11) runningLightsParams.dualMode = value.as<bool>();
        }
        else if (key == "sectionWidth" && value.is<uint8_t>()) {
            androidParams.sectionWidth = value.as<uint8_t>();
        }
        // Palette (for Twinkle, TwinkleFox, Fire, etc.)
        else if (key == "palette") {
            int p = value.as<int>();
            if (currentEffect == 7) wavyParams.palette = (PaletteType)p;
            else if (currentEffect == 13) twinkleParams.palette = (PaletteType)p;
            else if (currentEffect == 14) twinkleFoxParams.palette = (PaletteType)p;
            else if (currentEffect == 18) fireParams.palette = (PaletteType)p;
            else if (currentEffect == 22) auroraParams.palette = (PaletteType)p;
            else if (currentEffect == 23) pacificaParams.palette = (PaletteType)p;
            else if (currentEffect == 24) lakeParams.palette = (PaletteType)p;
            else if (currentEffect == 25) fairyParams.palette = (PaletteType)p;
            else if (currentEffect == 30) bouncingBallsParams.palette = (PaletteType)p;
            else if (currentEffect == 31) popcornParams.palette = (PaletteType)p;
        }
        else if (key == "fadeSpeed" && value.is<uint8_t>()) {
            twinkleParams.fadeSpeed = value.as<uint8_t>();
        }
        else if (key == "colorMode") {
            if (currentEffect == 13) {
                twinkleParams.colorMode = (TwinkleMode)value.as<int>();
            } else if (currentEffect == 25) {
                fairyParams.colorMode = (FairyMode)value.as<uint8_t>();
            }
        }
        else if (key == "twinkleColor" && value.is<const char*>()) {
            twinkleParams.twinkleColor = parseColor(value.as<const char*>());
        }
        else if (key == "twinkleRate" && value.is<uint8_t>()) {
            twinkleFoxParams.twinkleRate = value.as<uint8_t>();
        }
        // Sparkle specific
        else if (key == "colorSpark" && value.is<const char*>()) {
            sparkleParams.colorSpark = parseColor(value.as<const char*>());
        }
        else if (key == "overlay" && value.is<bool>()) {
            if (currentEffect == 15) sparkleParams.overlay = value.as<bool>();
            else if (currentEffect == 16) glitterParams.overlay = value.as<bool>();
            else if (currentEffect == 27) halloweenEyesParams.overlay = value.as<bool>();
            else if (currentEffect == 28) fireworksParams.overlay = value.as<bool>();
            else if (currentEffect == 32) dripParams.overlay = value.as<bool>();
            else if (currentEffect == 34) lightningParams.overlay = value.as<bool>();
        }
        else if (key == "darkMode" && value.is<bool>()) {
            sparkleParams.darkMode = value.as<bool>();
        }
        // Glitter specific
        else if (key == "rainbowBg" && value.is<bool>()) {
            glitterParams.rainbowBg = value.as<bool>();
        }
        // Starry Night specific
        else if (key == "density" && value.is<uint8_t>()) {
            if (currentEffect == 17) starryNightParams.density = value.as<uint8_t>();
            else if (currentEffect == 29) snowSparkleParams.density = value.as<uint8_t>();
        }
        else if (key == "colorStars" && value.is<const char*>()) {
            starryNightParams.colorStars = parseColor(value.as<const char*>());
        }
        else if (key == "shootingStars" && value.is<bool>()) {
            starryNightParams.shootingStars = value.as<bool>();
        }
        // Fire specific
        else if (key == "cooling" && value.is<uint8_t>()) {
            fireParams.cooling = value.as<uint8_t>();
        }
        else if (key == "sparking" && value.is<uint8_t>()) {
            fireParams.sparking = value.as<uint8_t>();
        }
        else if (key == "boost" && value.is<bool>()) {
            fireParams.boost = value.as<bool>();
        }
        // Candle specific
        else if (key == "multiMode" && value.is<bool>()) {
            candleParams.multiMode = value.as<bool>();
        }
        else if (key == "colorShift" && value.is<uint8_t>()) {
            candleParams.colorShift = value.as<uint8_t>();
        }
        // Lava specific (effect 21)
        else if (key == "blobSize" && value.is<uint8_t>()) {
            lavaParams.blobSize = value.as<uint8_t>();
        }
        else if (key == "smoothness" && value.is<uint8_t>()) {
            lavaParams.smoothness = value.as<uint8_t>();
        }
        // Fairy specific (effect 25)
        else if (key == "numFlashers" && value.is<uint8_t>()) {
            fairyParams.numFlashers = value.as<uint8_t>();
        }
        // ChristmasChase specific (effect 26) - color1/color2 handled earlier with other multi-color effects
        else if (key == "pattern" && value.is<uint8_t>()) {
            christmasChaseParams.pattern = (ChristmasPattern)value.as<uint8_t>();
        }
        // HalloweenEyes specific (effect 27)
        else if (key == "duration" && value.is<uint8_t>()) {
            halloweenEyesParams.duration = value.as<uint8_t>() * 10; // Scale to ms
        }
        else if (key == "fadeTime" && value.is<uint8_t>()) {
            halloweenEyesParams.fadeTime = value.as<uint8_t>() * 5; // Scale to ms
        }
        // Fireworks specific (effect 28)
        else if (key == "chance" && value.is<uint8_t>()) {
            fireworksParams.chance = value.as<uint8_t>();
        }
        else if (key == "fragments" && value.is<uint8_t>()) {
            fireworksParams.fragments = value.as<uint8_t>();
        }
        else if (key == "gravity" && value.is<uint8_t>()) {
            if (currentEffect == 28) fireworksParams.gravity = value.as<uint8_t>();
            else if (currentEffect == 30) bouncingBallsParams.gravity = value.as<uint8_t>();
            else if (currentEffect == 32) dripParams.gravity = value.as<uint8_t>();
        }
        // BouncingBalls specific (effect 30)
        else if (key == "numBalls" && value.is<uint8_t>()) {
            bouncingBallsParams.numBalls = constrain(value.as<uint8_t>(), 1, 8);
        }
        else if (key == "trail" && value.is<uint8_t>()) {
            bouncingBallsParams.trail = value.as<uint8_t>();
        }
        // Drip specific (effect 32)
        else if (key == "numDrips" && value.is<uint8_t>()) {
            dripParams.numDrips = constrain(value.as<uint8_t>(), 1, 8);
        }
        // Plasma specific (effect 33)
        else if (key == "phase" && value.is<uint8_t>()) {
            plasmaParams.phase = value.as<uint8_t>();
        }
        // Matrix specific (effect 35)
        else if (key == "spawningRate" && value.is<uint8_t>()) {
            matrixParams.spawningRate = value.as<uint8_t>();
        }
        // Heartbeat specific (effect 36)
        else if (key == "bpm" && value.is<uint8_t>()) {
            heartbeatParams.bpm = constrain(value.as<uint8_t>(), 40, 180);
        }
        // Breathe specific (effect 37)
        else if (key == "twoColor" && value.is<bool>()) {
            breatheParams.twoColor = value.as<bool>();
        }
        // Dissolve specific (effect 38)
        else if (key == "repeatSpeed" && value.is<uint8_t>()) {
            dissolveParams.repeatSpeed = value.as<uint8_t>();
        }
        else if (key == "dissolveSpeed" && value.is<uint8_t>()) {
            dissolveParams.dissolveSpeed = value.as<uint8_t>();
        }
        else if (key == "randomColors" && value.is<bool>()) {
            dissolveParams.randomColors = value.as<bool>();
        }
        // Fade specific (effect 39) - loop parameter
        else if (key == "loop" && value.is<bool>()) {
            fadeParams.loop = value.as<bool>();
        }
        // Strobe specific (effect 41) - mode parameter
        else if (key == "mode" && value.is<uint8_t>()) {
            strobeParams.mode = (StrobeMode)constrain(value.as<uint8_t>(), 0, 2);
        }
    }
    
    // ========================================================================
    // Getters
    // ========================================================================
    
    static uint8_t getCurrentEffect() { return currentEffect; }
    static bool isPoweredOn() { return powerOn; }
    static uint8_t getBrightness() { return brightness; }
    static const char* getEffectName() { return effects[currentEffect].name; }
    static uint8_t getNumEffects() { return NUM_EFFECTS; }
    
    // Get current effect params as JSON
    static void getStatusJson(JsonDocument& doc) {
        doc["power"] = powerOn;
        doc["brightness"] = brightness;
        doc["effect"] = currentEffect;
        doc["effectName"] = effects[currentEffect].name;
        doc["category"] = effects[currentEffect].category;
        doc["numEffects"] = NUM_EFFECTS;
    }
    
    // Get all effects list as JSON
    static void getEffectsJson(JsonDocument& doc) {
        JsonArray arr = doc.to<JsonArray>();
        for (uint8_t i = 0; i < NUM_EFFECTS; i++) {
            JsonObject obj = arr.add<JsonObject>();
            obj["id"] = i;
            obj["name"] = effects[i].name;
            obj["category"] = effects[i].category;
        }
    }
    
    // Get parameters for current effect
    static void getParamsJson(JsonDocument& doc) {
        // Return current effect's parameters
        doc["effect"] = currentEffect;
        
        JsonObject params = doc["params"].to<JsonObject>();
        
        // Add params based on current effect
        // This is a simplified version - full implementation would map all params
        switch (currentEffect) {
            case 0: // Solid
                params["color"] = colorToHex(solidParams.color);
                break;
            case 1: // Gradient
                params["colorStart"] = colorToHex(gradientParams.colorStart);
                params["colorMiddle"] = colorToHex(gradientParams.colorMiddle);
                params["colorEnd"] = colorToHex(gradientParams.colorEnd);
                params["style"] = gradientParams.style;
                params["threePoint"] = gradientParams.threePoint;
                break;
            case 2: // Spots
                params["color"] = colorToHex(spotsParams.color);
                params["spread"] = spotsParams.spread;
                params["width"] = spotsParams.width;
                params["fade"] = spotsParams.fade;
                break;
            case 3: // Pattern
                params["colorFg"] = colorToHex(patternParams.colorFg);
                params["colorBg"] = colorToHex(patternParams.colorBg);
                params["fgSize"] = patternParams.fgSize;
                params["bgSize"] = patternParams.bgSize;
                break;
            case 4: // Rainbow Wave
                params["speed"] = rainbowWaveParams.speed;
                params["size"] = rainbowWaveParams.size;
                params["saturation"] = rainbowWaveParams.saturation;
                break;
            case 5: // Color Wave
                params["color1"] = colorToHex(colorWaveParams.colors[0]);
                params["color2"] = colorToHex(colorWaveParams.colors[1]);
                params["color3"] = colorToHex(colorWaveParams.colors[2]);
                params["color4"] = colorToHex(colorWaveParams.colors[3]);
                params["color5"] = colorToHex(colorWaveParams.colors[4]);
                params["color6"] = colorToHex(colorWaveParams.colors[5]);
                params["color7"] = colorToHex(colorWaveParams.colors[6]);
                params["color8"] = colorToHex(colorWaveParams.colors[7]);
                params["numColors"] = colorWaveParams.numColors;
                params["direction"] = colorWaveParams.direction;
                params["speed"] = colorWaveParams.speed;
                break;
            case 6: // Oscillate
                params["colorPrimary"] = colorToHex(oscillateParams.colorPrimary);
                params["colorSecondary"] = colorToHex(oscillateParams.colorSecondary);
                params["speed"] = oscillateParams.speed;
                params["pointSize"] = oscillateParams.pointSize;
                break;
            case 7: // Wavy
                params["palette"] = wavyParams.palette;
                params["speed"] = wavyParams.speed;
                params["amplitude"] = wavyParams.amplitude;
                params["frequency"] = wavyParams.frequency;
                break;
            case 8: // Theater Chase
                params["color"] = colorToHex(theaterChaseParams.color);
                params["speed"] = theaterChaseParams.speed;
                params["gapSize"] = theaterChaseParams.gapSize;
                params["rainbowMode"] = theaterChaseParams.rainbowMode;
                break;
            case 9: // Scanner
                params["color1"] = colorToHex(scannerParams.colors[0]);
                params["color2"] = colorToHex(scannerParams.colors[1]);
                params["color3"] = colorToHex(scannerParams.colors[2]);
                params["color4"] = colorToHex(scannerParams.colors[3]);
                params["color5"] = colorToHex(scannerParams.colors[4]);
                params["color6"] = colorToHex(scannerParams.colors[5]);
                params["color7"] = colorToHex(scannerParams.colors[6]);
                params["color8"] = colorToHex(scannerParams.colors[7]);
                params["speed"] = scannerParams.speed;
                params["numDots"] = scannerParams.numDots;
                params["trailLength"] = scannerParams.trailLength;
                params["dualMode"] = scannerParams.dualMode;
                break;
            case 10: // Comet
                params["color"] = colorToHex(cometParams.color);
                params["sparkleColor"] = colorToHex(cometParams.sparkleColor);
                params["speed"] = cometParams.speed;
                params["trailLength"] = cometParams.trailLength;
                params["sparkleEnabled"] = cometParams.sparkleEnabled;
                params["direction"] = cometParams.direction;
                break;
            case 11: // Running Lights
                params["color1"] = colorToHex(runningLightsParams.colors[0]);
                params["color2"] = colorToHex(runningLightsParams.colors[1]);
                params["color3"] = colorToHex(runningLightsParams.colors[2]);
                params["color4"] = colorToHex(runningLightsParams.colors[3]);
                params["numColors"] = runningLightsParams.numColors;
                params["speed"] = runningLightsParams.speed;
                params["waveWidth"] = runningLightsParams.waveWidth;
                params["shape"] = runningLightsParams.shape;
                params["dualMode"] = runningLightsParams.dualMode;
                break;
            case 12: // Android
                params["colorPrimary"] = colorToHex(androidParams.colorPrimary);
                params["colorSecondary"] = colorToHex(androidParams.colorSecondary);
                params["speed"] = androidParams.speed;
                params["sectionWidth"] = androidParams.sectionWidth;
                break;
            case 13: // Twinkle
                params["palette"] = twinkleParams.palette;
                params["twinkleColor"] = colorToHex(twinkleParams.twinkleColor);
                params["speed"] = twinkleParams.speed;
                params["intensity"] = twinkleParams.intensity;
                params["fadeSpeed"] = twinkleParams.fadeSpeed;
                params["colorMode"] = twinkleParams.colorMode;
                break;
            case 14: // TwinkleFox
                params["palette"] = twinkleFoxParams.palette;
                params["speed"] = twinkleFoxParams.speed;
                params["twinkleRate"] = twinkleFoxParams.twinkleRate;
                break;
            case 15: // Sparkle
                params["colorSpark"] = colorToHex(sparkleParams.colorSpark);
                params["colorBg"] = colorToHex(sparkleParams.colorBg);
                params["speed"] = sparkleParams.speed;
                params["intensity"] = sparkleParams.intensity;
                params["overlay"] = sparkleParams.overlay;
                params["darkMode"] = sparkleParams.darkMode;
                break;
            case 16: // Glitter
                params["intensity"] = glitterParams.intensity;
                params["rainbowBg"] = glitterParams.rainbowBg;
                params["colorBg"] = colorToHex(glitterParams.bgColor);
                params["overlay"] = glitterParams.overlay;
                break;
            case 17: // Starry Night
                params["speed"] = starryNightParams.speed;
                params["density"] = starryNightParams.density;
                params["colorStars"] = colorToHex(starryNightParams.colorStars);
                params["shootingStars"] = starryNightParams.shootingStars;
                break;
            case 18: // Fire
                params["cooling"] = fireParams.cooling;
                params["sparking"] = fireParams.sparking;
                params["boost"] = fireParams.boost;
                params["palette"] = fireParams.palette;
                break;
            case 19: // Candle
                params["speed"] = candleParams.speed;
                params["intensity"] = candleParams.intensity;
                params["multiMode"] = candleParams.multiMode;
                params["color"] = colorToHex(candleParams.color);
                params["colorShift"] = candleParams.colorShift;
                break;
            case 20: // FireFlicker
                params["speed"] = fireFlickerParams.speed;
                params["intensity"] = fireFlickerParams.intensity;
                params["color"] = colorToHex(fireFlickerParams.color);
                break;
            case 21: // Lava
                params["speed"] = lavaParams.speed;
                params["blobSize"] = lavaParams.blobSize;
                params["smoothness"] = lavaParams.smoothness;
                break;
            case 22: // Aurora
                params["speed"] = auroraParams.speed;
                params["intensity"] = auroraParams.intensity;
                params["palette"] = auroraParams.palette;
                break;
            case 23: // Pacifica
                params["speed"] = pacificaParams.speed;
                params["palette"] = pacificaParams.palette;
                break;
            case 24: // Lake
                params["speed"] = lakeParams.speed;
                params["palette"] = lakeParams.palette;
                break;
            case 25: // Fairy
                params["speed"] = fairyParams.speed;
                params["numFlashers"] = fairyParams.numFlashers;
                params["colorMode"] = fairyParams.colorMode;
                params["palette"] = fairyParams.palette;
                break;
            case 26: // ChristmasChase
                params["speed"] = christmasChaseParams.speed;
                params["color1"] = colorToHex(christmasChaseParams.color1);
                params["color2"] = colorToHex(christmasChaseParams.color2);
                params["pattern"] = christmasChaseParams.pattern;
                break;
            case 27: // HalloweenEyes
                params["duration"] = halloweenEyesParams.duration / 10;
                params["fadeTime"] = halloweenEyesParams.fadeTime / 5;
                params["color"] = colorToHex(halloweenEyesParams.color);
                params["overlay"] = halloweenEyesParams.overlay;
                break;
            case 28: // Fireworks
                params["chance"] = fireworksParams.chance;
                params["fragments"] = fireworksParams.fragments;
                params["gravity"] = fireworksParams.gravity;
                params["overlay"] = fireworksParams.overlay;
                break;
            case 29: // SnowSparkle
                params["speed"] = snowSparkleParams.speed;
                params["density"] = snowSparkleParams.density;
                params["color"] = colorToHex(snowSparkleParams.color);
                params["direction"] = snowSparkleParams.direction;
                break;
            case 30: // BouncingBalls
                params["gravity"] = bouncingBallsParams.gravity;
                params["numBalls"] = bouncingBallsParams.numBalls;
                params["trail"] = bouncingBallsParams.trail;
                params["palette"] = bouncingBallsParams.palette;
                break;
            case 31: // Popcorn
                params["speed"] = popcornParams.speed;
                params["intensity"] = popcornParams.intensity;
                params["palette"] = popcornParams.palette;
                break;
            case 32: // Drip
                params["gravity"] = dripParams.gravity;
                params["numDrips"] = dripParams.numDrips;
                params["color"] = colorToHex(dripParams.color);
                params["overlay"] = dripParams.overlay;
                break;
            case 33: // Plasma
                params["phase"] = plasmaParams.phase;
                params["intensity"] = plasmaParams.intensity;
                params["speed"] = plasmaParams.speed;
                break;
            case 34: // Lightning
                params["frequency"] = lightningParams.frequency;
                params["intensity"] = lightningParams.intensity;
                params["color"] = colorToHex(lightningParams.color);
                params["overlay"] = lightningParams.overlay;
                break;
            case 35: // Matrix
                params["speed"] = matrixParams.speed;
                params["spawningRate"] = matrixParams.spawningRate;
                params["trailLength"] = matrixParams.trailLength;
                params["color"] = colorToHex(matrixParams.color);
                break;
            case 36: // Heartbeat
                params["bpm"] = heartbeatParams.bpm;
                params["color"] = colorToHex(heartbeatParams.color);
                break;
            case 37: // Breathe
                params["speed"] = breatheParams.speed;
                params["colorPrimary"] = colorToHex(breatheParams.colorPrimary);
                params["colorSecondary"] = colorToHex(breatheParams.colorSecondary);
                params["twoColor"] = breatheParams.twoColor;
                break;
            case 38: // Dissolve
                params["repeatSpeed"] = dissolveParams.repeatSpeed;
                params["dissolveSpeed"] = dissolveParams.dissolveSpeed;
                params["randomColors"] = dissolveParams.randomColors;
                params["color"] = colorToHex(dissolveParams.color);
                break;
            case 39: // Fade
                params["speed"] = fadeParams.speed;
                params["color1"] = colorToHex(fadeParams.colors[0]);
                params["color2"] = colorToHex(fadeParams.colors[1]);
                params["color3"] = colorToHex(fadeParams.colors[2]);
                params["color4"] = colorToHex(fadeParams.colors[3]);
                params["color5"] = colorToHex(fadeParams.colors[4]);
                params["color6"] = colorToHex(fadeParams.colors[5]);
                params["color7"] = colorToHex(fadeParams.colors[6]);
                params["color8"] = colorToHex(fadeParams.colors[7]);
                params["numColors"] = fadeParams.numColors;
                params["loop"] = fadeParams.loop;
                break;
            case 40: // Police
                params["speed"] = policeLightsParams.speed;
                params["color1"] = colorToHex(policeLightsParams.color1);
                params["color2"] = colorToHex(policeLightsParams.color2);
                params["style"] = policeLightsParams.style;
                break;
            case 41: // Strobe
                params["frequency"] = strobeParams.frequency;
                params["color"] = colorToHex(strobeParams.color);
                params["mode"] = strobeParams.mode;
                break;
            default:
                break;
        }
    }

private:
    static TaskHandle_t ledTaskHandle;
    static uint8_t currentEffect;
    static uint8_t brightness;
    static bool powerOn;
    static bool effectChanged;
    static bool effectReady;  // True after first setEffect() call
    static uint32_t frameCounter;
    static uint32_t lastFrameTime;
    
    // Effect function array
    static const EffectEntry effects[];
    static const uint8_t NUM_EFFECTS;
    
    // ========================================================================
    // FreeRTOS Task
    // ========================================================================
    
    static void ledTask(void* params) {
        const TickType_t frameDelay = pdMS_TO_TICKS(1000 / LED_TARGET_FPS);
        TickType_t lastWakeTime = xTaskGetTickCount();
        
        // Crossfade state for smooth startup transition
        static bool firstRun = true;
        static uint16_t crossfadeProgress = 256;  // Start at 256 = no crossfade active
        static CRGB previousLeds[ARGB_NUM_LEDS];
        
        LOG_INFO("LED Task started on Core 0");
        
        while (true) {
            if (powerOn && effectReady) {
                // Handle effect change or first run
                if (effectChanged) {
                    if (firstRun) {
                        // Save current LED state for crossfade
                        memcpy(previousLeds, leds, sizeof(leds));
                        crossfadeProgress = 0;  // Start crossfade
                        firstRun = false;
                    } else {
                        // Normal effect change - clear LEDs
                        FastLED.clear();
                    }
                    frameCounter = 0;
                    effectChanged = false;
                }
                
                // Execute current effect into leds[]
                if (currentEffect < NUM_EFFECTS) {
                    effects[currentEffect].func();
                }
                
                // Apply crossfade if in progress (0-255)
                if (crossfadeProgress < 256) {
                    uint8_t blendAmount = (crossfadeProgress > 255) ? 255 : crossfadeProgress;
                    for (uint16_t i = 0; i < ARGB_NUM_LEDS; i++) {
                        leds[i] = blend(previousLeds[i], leds[i], blendAmount);
                    }
                    crossfadeProgress += 8;  // ~30 frames = 500ms crossfade
                }
                
                // Show LEDs
                FastLED.show();
                
                frameCounter++;
                lastFrameTime = millis();
            }
            
            // Maintain consistent frame rate
            vTaskDelayUntil(&lastWakeTime, frameDelay);
        }
    }
    
    // ========================================================================
    // Parameter Helpers
    // ========================================================================
    
    static CRGB parseColor(const char* hex) {
        if (hex[0] == '#') hex++;
        uint32_t val = strtoul(hex, NULL, 16);
        return CRGB((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
    }
    
    static String colorToHex(CRGB color) {
        char buf[8];
        sprintf(buf, "#%02X%02X%02X", color.r, color.g, color.b);
        return String(buf);
    }
    
    static void applySpeedParam(uint8_t speed) {
        // Apply speed to current effect's params
        switch (currentEffect) {
            case 4: rainbowWaveParams.speed = speed; break;
            case 5: colorWaveParams.speed = speed; break;
            case 6: oscillateParams.speed = speed; break;
            case 7: wavyParams.speed = speed; break;
            case 8: theaterChaseParams.speed = speed; break;
            case 9: scannerParams.speed = speed; break;
            case 10: cometParams.speed = speed; break;
            case 11: runningLightsParams.speed = speed; break;
            case 12: androidParams.speed = speed; break;
            case 13: twinkleParams.speed = speed; break;
            case 14: twinkleFoxParams.speed = speed; break;
            case 15: sparkleParams.speed = speed; break;
            case 17: starryNightParams.speed = speed; break;
            case 19: candleParams.speed = speed; break;
            case 20: fireFlickerParams.speed = speed; break;
            case 21: lavaParams.speed = speed; break;
            case 22: auroraParams.speed = speed; break;
            case 23: pacificaParams.speed = speed; break;
            case 24: lakeParams.speed = speed; break;
            case 25: fairyParams.speed = speed; break;
            case 26: christmasChaseParams.speed = speed; break;
            case 29: snowSparkleParams.speed = speed; break;
            case 31: popcornParams.speed = speed; break;
            case 33: plasmaParams.speed = speed; break;
            case 35: matrixParams.speed = speed; break;
            case 37: breatheParams.speed = speed; break;
            case 39: fadeParams.speed = speed; break;
            case 40: policeLightsParams.speed = speed; break;
            default: break;
        }
    }
    
    static void applyColorParam(CRGB color) {
        switch (currentEffect) {
            case 0: solidParams.color = color; break;
            case 2: spotsParams.color = color; break;
            case 8: theaterChaseParams.color = color; break;
            case 9: scannerParams.colors[0] = color; break;
            case 10: cometParams.color = color; break;
            case 11: runningLightsParams.colors[0] = color; break;
            case 19: candleParams.color = color; break;
            case 20: fireFlickerParams.color = color; break;
            case 27: halloweenEyesParams.color = color; break;
            case 29: snowSparkleParams.color = color; break;
            case 32: dripParams.color = color; break;
            case 34: lightningParams.color = color; break;
            case 35: matrixParams.color = color; break;
            case 36: heartbeatParams.color = color; break;
            case 38: dissolveParams.color = color; break;
            case 41: strobeParams.color = color; break;
            default: break;
        }
    }
    
    static void applyIntensityParam(uint8_t intensity) {
        switch (currentEffect) {
            case 13: twinkleParams.intensity = intensity; break;
            case 15: sparkleParams.intensity = intensity; break;
            case 16: glitterParams.intensity = intensity; break;
            case 19: candleParams.intensity = intensity; break;
            case 20: fireFlickerParams.intensity = intensity; break;
            case 22: auroraParams.intensity = intensity; break;
            case 31: popcornParams.intensity = intensity; break;
            case 33: plasmaParams.intensity = intensity; break;
            case 34: lightningParams.intensity = intensity; break;
            default: break;
        }
    }
};

// ============================================================================
// Static Member Initialization
// ============================================================================

TaskHandle_t LEDController::ledTaskHandle = NULL;
uint8_t LEDController::currentEffect = 0;
uint8_t LEDController::brightness = 180;
bool LEDController::powerOn = true;
bool LEDController::effectChanged = true;
bool LEDController::effectReady = false;  // Wait for setEffect() before running
uint32_t LEDController::frameCounter = 0;
uint32_t LEDController::lastFrameTime = 0;

// Effect function array
const LEDController::EffectEntry LEDController::effects[] = {
    // Category 1: Static
    {"Solid", effectSolid, 1},
    {"Gradient", effectGradient, 1},
    {"Spots", effectSpots, 1},
    {"Pattern", effectPattern, 1},
    
    // Category 2: Wave/Fale
    {"Rainbow Wave", effectRainbowWave, 2},
    {"Color Wave", effectColorWave, 2},
    {"Oscillate", effectOscillate, 2},
    {"Wavy", effectWavy, 2},
    
    // Category 3: Chase/Running
    {"Theater Chase", effectTheaterChase, 3},
    {"Scanner", effectScanner, 3},
    {"Comet", effectComet, 3},
    {"Running Lights", effectRunningLights, 3},
    {"Android", effectAndroid, 3},
    
    // Category 4: Twinkle/Sparkle
    {"Twinkle", effectTwinkle, 4},
    {"TwinkleFox", effectTwinkleFox, 4},
    {"Sparkle", effectSparkle, 4},
    {"Glitter", effectGlitter, 4},
    {"Starry Night", effectStarryNight, 4},
    
    // Category 5: Fire/Organic
    {"Fire", effectFire, 5},
    {"Candle", effectCandle, 5},
    {"Fire Flicker", effectFireFlicker, 5},
    {"Lava", effectLava, 5},
    {"Aurora", effectAurora, 5},
    {"Pacifica", effectPacifica, 5},
    {"Lake", effectLake, 5},
    
    // Category 6: Christmas/Seasonal
    {"Fairy Lights", effectFairy, 6},
    {"Christmas Chase", effectChristmasChase, 6},
    {"Halloween Eyes", effectHalloweenEyes, 6},
    {"Fireworks", effectFireworks, 6},
    {"Snow Sparkle", effectSnowSparkle, 6},
    
    // Category 7: Special
    {"Bouncing Balls", effectBouncingBalls, 7},
    {"Popcorn", effectPopcorn, 7},
    {"Drip", effectDrip, 7},
    {"Plasma", effectPlasma, 7},
    {"Lightning", effectLightning, 7},
    {"Matrix", effectMatrix, 7},
    {"Heartbeat", effectHeartbeat, 7},
    
    // Category 8: Breathing/Fade
    {"Breathe", effectBreathe, 8},
    {"Dissolve", effectDissolve, 8},
    {"Fade", effectFade, 8},
    
    // Category 9: Alarm
    {"Police Lights", effectPolice, 9},
    {"Strobe", effectStrobe, 9}
};

const uint8_t LEDController::NUM_EFFECTS = sizeof(LEDController::effects) / sizeof(LEDController::effects[0]);

#endif // LED_CONTROLLER_H
