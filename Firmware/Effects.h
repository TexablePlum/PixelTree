/*
 * Effects.h - Implementation of all LED effects
 */

#ifndef EFFECTS_H
#define EFFECTS_H

#include <FastLED.h>
#include "EffectParams.h"
#include "Palettes.h"

// Configuration constants
#define NUM_LEDS 75

// Forward declarations
void effectSolid();
void effectGradient();
void effectSpots();
void effectPattern();
void effectRainbowWave();
void effectColorWave();
void effectOscillate();
void effectWavy();
void effectTheaterChase();
void effectScanner();
void effectComet();
void effectRunningLights();
void effectAndroid();
void effectTwinkle();
void effectTwinkleFox();
void effectSparkle();
void effectGlitter();
void effectStarryNight();
void effectFire();
void effectCandle();
void effectFireFlicker();
void effectLava();
void effectAurora();
void effectPacifica();
void effectLake();
void effectFairy();
void effectChristmasChase();
void effectHalloweenEyes();
void effectFireworks();
void effectSnowSparkle();
void effectBouncingBalls();
void effectPopcorn();
void effectDrip();
void effectPlasma();
void effectLightning();
void effectMatrix();
void effectHeartbeat();
void effectBreathe();
void effectDissolve();
void effectFade();
void effectPolice();
void effectStrobe();

// Helper functions
uint16_t mapLed(uint16_t pos, Direction dir);
void setLedSafe(uint16_t pos, CRGB color);
void addLedSafe(uint16_t pos, CRGB color);
void blendLedSafe(uint16_t pos, CRGB color, uint8_t amount);
void fadeAll(uint8_t amount);
CRGB getColorFromPalette(PaletteType paletteType, uint8_t index, uint8_t brightness);

// ============================================================================
// CATEGORY 1: STATIC EFFECTS
// ============================================================================

void effectSolid() {
    // Simplest effect - solid color
    CRGB col = solidParams.color;
    col.nscale8(solidParams.brightness);
    fill_solid(leds, NUM_LEDS, col);
}

void effectGradient() {
    // Apply style-specific gradient
    if (gradientParams.style == GRADIENT_MIRROR) {
        // MIRROR: symmetric gradient
        uint8_t half = NUM_LEDS / 2;
        
        if (gradientParams.threePoint) {
            // 3-point mirror: create full gradient on first half, then mirror
            // First quarter: colorStart -> colorMiddle
            uint8_t quarter = half / 2;
            if (quarter > 0) {
                fill_gradient_RGB(leds, 0, gradientParams.colorStart, 
                                 quarter, gradientParams.colorMiddle);
            }
            // Second quarter: colorMiddle -> colorEnd
            if (quarter < half) {
                fill_gradient_RGB(leds, quarter, gradientParams.colorMiddle,
                                 half, gradientParams.colorEnd);
            }
        } else {
            // 2-point mirror: colorStart -> colorEnd on first half
            fill_gradient_RGB(leds, 0, gradientParams.colorStart, 
                             half, gradientParams.colorEnd);
        }
        
        // Mirror the first half to second half
        for (uint16_t i = 0; i < half; i++) {
            leds[NUM_LEDS - 1 - i] = leds[i];
        }
    }
    else if (gradientParams.style == GRADIENT_SCATTERED) {
        // SCATTERED: random-looking gradient with color clusters
        if (gradientParams.threePoint) {
            // Divide strip into random segments with different colors
            uint8_t third = NUM_LEDS / 3;
            
            // Create 3 sections with smooth transitions
            fill_gradient_RGB(leds, 0, gradientParams.colorStart,
                             third, gradientParams.colorMiddle);
            fill_gradient_RGB(leds, third, gradientParams.colorEnd,
                             third * 2, gradientParams.colorStart);
            fill_gradient_RGB(leds, third * 2, gradientParams.colorMiddle,
                             NUM_LEDS - 1, gradientParams.colorEnd);
        } else {
            // 2-point: alternate gradient with middle mix
            uint8_t third = NUM_LEDS / 3;
            CRGB mixColor = blend(gradientParams.colorStart, gradientParams.colorEnd, 128);
            
            fill_gradient_RGB(leds, 0, gradientParams.colorEnd, third, mixColor);
            fill_gradient_RGB(leds, third, gradientParams.colorStart, third * 2, mixColor);
            fill_gradient_RGB(leds, third * 2, mixColor, NUM_LEDS - 1, gradientParams.colorEnd);
        }
    }
    else {
        // LINEAR (default): normal gradient
        if (gradientParams.threePoint) {
            uint8_t midPoint = NUM_LEDS / 2;
            // colorStart -> colorMiddle -> colorEnd
            if (midPoint > 0) {
                fill_gradient_RGB(leds, 0, gradientParams.colorStart, 
                                 midPoint, gradientParams.colorMiddle);
            }
            if (midPoint < NUM_LEDS - 1) {
                fill_gradient_RGB(leds, midPoint, gradientParams.colorMiddle, 
                                 NUM_LEDS - 1, gradientParams.colorEnd);
            }
        } else {
            // colorStart -> colorEnd
            fill_gradient_RGB(leds, NUM_LEDS, gradientParams.colorStart, gradientParams.colorEnd);
        }
    }
}

void effectSpots() {
    FastLED.clear();
    
    for (uint16_t i = 0; i < NUM_LEDS; i += spotsParams.spread) {
        for (uint8_t w = 0; w < spotsParams.width && (i + w) < NUM_LEDS; w++) {
            if (spotsParams.fade && spotsParams.width > 1) {
                // Fading edges
                uint8_t brightness;
                uint8_t halfWidth = spotsParams.width / 2;
                
                // Prevent map() crash when halfWidth == 0 or width-1 == halfWidth
                if (halfWidth == 0 || spotsParams.width <= 2) {
                    // Too narrow for gradient, use full brightness
                    brightness = 255;
                } else if (w < halfWidth) {
                    brightness = map(w, 0, halfWidth - 1, 64, 255);
                } else {
                    brightness = map(w, halfWidth, spotsParams.width - 1, 255, 64);
                }
                
                CRGB col = spotsParams.color;
                col.nscale8(brightness);
                leds[i + w] = col;
            } else {
                leds[i + w] = spotsParams.color;
            }
        }
    }
}

void effectPattern() {
    uint8_t patternLen = patternParams.fgSize + patternParams.bgSize;
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint8_t pos = i % patternLen;
        if (pos < patternParams.fgSize) {
            leds[i] = patternParams.colorFg;
        } else {
            leds[i] = patternParams.colorBg;
        }
    }
}

// ============================================================================
// CATEGORY 2: WAVE EFFECTS
// ============================================================================

void effectRainbowWave() {
    static uint16_t hueOffset = 0;
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint16_t pos = mapLed(i, rainbowWaveParams.direction);
        uint8_t hue = (pos * 256 / rainbowWaveParams.size + hueOffset) & 0xFF;
        leds[i] = CHSV(hue, rainbowWaveParams.saturation, 255);
    }
    
    hueOffset += map(rainbowWaveParams.speed, 0, 255, 1, 10);
}

void effectColorWave() {
    static float offset = 0;
    
    // Prevent division by zero
    if (colorWaveParams.numColors == 0) return;
    
    uint16_t segmentLen = NUM_LEDS / colorWaveParams.numColors;
    if (segmentLen == 0) segmentLen = 1; // Safety check
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint16_t pos = mapLed(i, colorWaveParams.direction);
        uint16_t adjustedPos = ((uint16_t)(pos + offset)) % NUM_LEDS;
        
        uint8_t colorIdx = adjustedPos / segmentLen;
        uint8_t nextColorIdx = (colorIdx + 1) % colorWaveParams.numColors;
        
        // Safe blend calculation
        uint8_t blendAmount;
        if (segmentLen > 1) {
            blendAmount = map(adjustedPos % segmentLen, 0, segmentLen - 1, 0, 255);
        } else {
            blendAmount = 0;
        }
        
        if (colorIdx < colorWaveParams.numColors) {
            leds[i] = blend(colorWaveParams.colors[colorIdx], 
                           colorWaveParams.colors[nextColorIdx], 
                           blendAmount);
        }
    }
    
    // Normalize speed: higher numColors = smaller segments, so scale offset increment
    // This keeps visual wave speed constant regardless of number of colors
    float speedFactor = map(colorWaveParams.speed, 0, 255, 10, 100) / 100.0;
    float normalizedIncrement = speedFactor * (float)segmentLen / 10.0;
    
    offset += normalizedIncrement;
    if (offset >= NUM_LEDS) offset -= NUM_LEDS;
}

void effectOscillate() {
    static int16_t position = 0;
    static int8_t direction = 1;
    static uint32_t lastMove = 0;
    
    uint16_t delayMs = map(oscillateParams.speed, 0, 255, 80, 5);
    
    if (millis() - lastMove > delayMs) {
        position += direction;
        if (position >= NUM_LEDS - 1 || position <= 0) {
            direction = -direction;
        }
        lastMove = millis();
    }
    
    // Fade trail effect - softer fade for brightness
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i].nscale8(220); // 86% brightness retention, gentler fade
    }
    
    // Color based on position: left side = colorPrimary, right side = colorSecondary
    // Use position-weighted color (no HSV to avoid hue rotation issues)
    CRGB pointColor;
    if (position < NUM_LEDS / 2) {
        // Left half - mostly colorPrimary
        uint8_t blendAmt = map(position, 0, NUM_LEDS / 2, 0, 128);
        pointColor = blend(oscillateParams.colorPrimary, oscillateParams.colorSecondary, blendAmt);
    } else {
        // Right half - mostly colorSecondary
        uint8_t blendAmt = map(position, NUM_LEDS / 2, NUM_LEDS - 1, 128, 255);
        pointColor = blend(oscillateParams.colorPrimary, oscillateParams.colorSecondary, blendAmt);
    }
    
    // Draw bouncing point with glow based on pointSize
    uint8_t size = oscillateParams.pointSize;
    if (size < 1) size = 1;
    if (size > 20) size = 20;
    
    for (int8_t offset = -size; offset <= size; offset++) {
        int16_t ledPos = position + offset;
        if (ledPos >= 0 && ledPos < NUM_LEDS) {
            uint8_t brightness = map(abs(offset), 0, size, 255, 0);
            CRGB col = pointColor;
            col.nscale8(brightness);
            leds[ledPos] = col;
        }
    }
}

void effectWavy() {
    static uint16_t phase = 0;
    CRGBPalette16 pal = getPalette(wavyParams.palette);
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        // Sinusoid with multiple waves
        uint8_t sinVal = sin8(i * wavyParams.frequency * 256 / NUM_LEDS + phase);
        uint8_t brightness = scale8(sinVal, wavyParams.amplitude);
        
        uint8_t colorIndex = i * 256 / NUM_LEDS + phase / 2;
        leds[i] = ColorFromPalette(pal, colorIndex, brightness + (255 - wavyParams.amplitude), LINEARBLEND);
    }
    
    phase += map(wavyParams.speed, 0, 255, 1, 8);
}

// ============================================================================
// CATEGORY 3: CHASE/RUNNING EFFECTS
// ============================================================================

void effectTheaterChase() {
    static uint8_t step = 0;
    static uint32_t lastStep = 0;
    static uint8_t hue = 0;
    
    uint16_t delayMs = map(theaterChaseParams.speed, 0, 255, 150, 20);
    
    if (millis() - lastStep > delayMs) {
        step = (step + 1) % (theaterChaseParams.gapSize + 1);
        if (theaterChaseParams.rainbowMode) {
            hue += 2;
        }
        lastStep = millis();
    }
    
    FastLED.clear();
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        if ((i + step) % (theaterChaseParams.gapSize + 1) == 0) {
            if (theaterChaseParams.rainbowMode) {
                leds[i] = CHSV(hue + i * 2, 255, 255);
            } else {
                leds[i] = theaterChaseParams.color;
            }
        }
    }
}

void effectScanner() {
    static int16_t positions[8] = {0};
    static int8_t directions[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    static uint32_t lastMove = 0;
    static bool initialized = false;
    
    if (!initialized) {
        // Distribute dots evenly
        for (uint8_t i = 0; i < scannerParams.numDots; i++) {
            positions[i] = i * (NUM_LEDS / scannerParams.numDots);
        }
        initialized = true;
    }
    
    uint16_t delayMs = map(scannerParams.speed, 0, 255, 80, 10);
    
    // Fade
    if (!scannerParams.overlay) {
        uint8_t fadeAmount = map(scannerParams.trailLength, 1, 50, 100, 20);
        fadeAll(fadeAmount);
    }
    
    if (millis() - lastMove > delayMs) {
        for (uint8_t d = 0; d < scannerParams.numDots; d++) {
            positions[d] += directions[d];
            
            if (positions[d] >= NUM_LEDS - 1) {
                positions[d] = NUM_LEDS - 1;
                directions[d] = -1;
            } else if (positions[d] <= 0) {
                positions[d] = 0;
                directions[d] = 1;
            }
        }
        lastMove = millis();
    }
    
    // Draw dots - each dot has its own color
    for (uint8_t d = 0; d < scannerParams.numDots; d++) {
        if (positions[d] >= 0 && positions[d] < NUM_LEDS) {
            leds[positions[d]] = scannerParams.colors[d % 8]; // Modulo 8 for safety
        }
    }
    
    // Dual mode - second set from the other side
    if (scannerParams.dualMode) {
        for (uint8_t d = 0; d < scannerParams.numDots; d++) {
            int16_t mirrorPos = NUM_LEDS - 1 - positions[d];
            if (mirrorPos >= 0 && mirrorPos < NUM_LEDS) {
                leds[mirrorPos] = scannerParams.colors[d % 8]; // Same color for mirrored dot
            }
        }
    }
}

void effectComet() {
    static int16_t position = 0;
    static uint32_t lastMove = 0;
    static uint8_t sparkles[100]; // Sparkle brightness for each position
    
    uint16_t delayMs = map(cometParams.speed, 0, 255, 60, 5);
    
    // Fade existing sparkles FAST
    for (uint16_t i = 0; i < NUM_LEDS && i < 100; i++) {
        if (sparkles[i] > 50) sparkles[i] -= 50; // Very fast fade
        else sparkles[i] = 0;
    }
    
    if (millis() - lastMove > delayMs) {
        if (cometParams.direction == DIR_FORWARD) {
            position++;
            if (position >= NUM_LEDS + cometParams.trailLength) {
                position = -cometParams.trailLength;
            }
        } else {
            position--;
            if (position < -cometParams.trailLength) {
                position = NUM_LEDS + cometParams.trailLength;
            }
        }
        lastMove = millis();
    }
    
    FastLED.clear();
    
    // Draw comet with trail
    for (int16_t i = 0; i < cometParams.trailLength; i++) {
        int16_t ledPos;
        if (cometParams.direction == DIR_FORWARD) {
            ledPos = position - i;
        } else {
            ledPos = position + i;
        }
        
        if (ledPos >= 0 && ledPos < NUM_LEDS) {
            float ratio = (float)i / cometParams.trailLength;
            uint8_t brightness = 255 * (1.0 - ratio * ratio * ratio);
            
            CRGB col = cometParams.color;
            col.nscale8(brightness);
            leds[ledPos] = col;
            
            // Occasionally create sparkle in the trail
            if (i > 4 && cometParams.sparkleEnabled && ledPos < 100) {
                if (random8() < 12) { // Low chance
                    sparkles[ledPos] = 255;
                }
            }
        }
    }
    
    // Draw sparkles - REPLACE pixel instead of adding
    if (cometParams.sparkleEnabled) {
        for (uint16_t i = 0; i < NUM_LEDS && i < 100; i++) {
            if (sparkles[i] > 30) {
                // Replace with sparkle color (not add)
                leds[i] = cometParams.sparkleColor;
                leds[i].nscale8(sparkles[i]);
            }
        }
    }
}

void effectRunningLights() {
    static uint16_t offset = 0;
    static uint32_t lastStep = 0;
    
    uint16_t delayMs = map(runningLightsParams.speed, 0, 255, 80, 10);
    
    if (millis() - lastStep > delayMs) {
        offset++;
        lastStep = millis();
    }
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint8_t wave;
        uint16_t phase = (i * 256 / runningLightsParams.waveWidth) + offset * 8;
        
        switch (runningLightsParams.shape) {
            case SHAPE_SINE:
                wave = sin8(phase);
                break;
            case SHAPE_SAW:
                wave = phase & 0xFF;
                break;
            case SHAPE_SQUARE:
                wave = (sin8(phase) > 127) ? 255 : 0;
                break;
            case SHAPE_TRIANGLE:
                wave = triwave8(phase);
                break;
            default:
                wave = sin8(phase);
        }
        
        // Color cycles: divide strip into numColors sections that shift with animation
        uint8_t numC = runningLightsParams.numColors;
        if (numC < 1) numC = 1;
        if (numC > 4) numC = 4;
        uint8_t colorIndex = ((i + offset) * numC / NUM_LEDS) % numC;
        CRGB col = runningLightsParams.colors[colorIndex];
        col.nscale8(wave);
        leds[i] = col;
    }
    
    // Dual mode - reverse wave overlaid
    if (runningLightsParams.dualMode) {
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            uint8_t wave;
            uint16_t phase = (i * 256 / runningLightsParams.waveWidth) - offset * 8;
            wave = sin8(phase);
            
            uint8_t numC = runningLightsParams.numColors;
            if (numC < 1) numC = 1;
            uint8_t colorIndex = ((i - offset) * numC / NUM_LEDS) % numC;
            CRGB col = runningLightsParams.colors[colorIndex];
            col.nscale8(wave / 2); // Dimmer for dual mode
            leds[i] += col;
        }
    }
}

void effectAndroid() {
    static int16_t position = 0;
    static int8_t direction = 1;
    static uint32_t lastMove = 0;
    
    uint16_t sectionLen = NUM_LEDS * androidParams.sectionWidth / 100;
    if (sectionLen < 3) sectionLen = 3;
    
    uint16_t delayMs = map(androidParams.speed, 0, 255, 50, 5);
    
    if (millis() - lastMove > delayMs) {
        position += direction;
        if (position + sectionLen >= NUM_LEDS) {
            direction = -1;
        } else if (position <= 0) {
            direction = 1;
        }
        lastMove = millis();
    }
    
    fill_solid(leds, NUM_LEDS, androidParams.colorSecondary);
    
    for (uint16_t i = 0; i < sectionLen; i++) {
        if (position + i >= 0 && position + i < NUM_LEDS) {
            leds[position + i] = androidParams.colorPrimary;
        }
    }
}

// ============================================================================
// CATEGORY 4: TWINKLE/SPARKLE EFFECTS
// ============================================================================

// State for twinkle effects
static uint8_t twinkleState[NUM_LEDS];
static uint8_t twinkleBrightness[NUM_LEDS];
static CRGB twinkleColors[NUM_LEDS];

void effectTwinkle() {
    static uint32_t lastUpdate = 0;
    static bool initialized = false;
    
    CRGBPalette16 pal = getPalette(twinkleParams.palette);
    
    if (!initialized) {
        memset(twinkleState, 0, NUM_LEDS);
        memset(twinkleBrightness, 0, NUM_LEDS);
        initialized = true;
    }
    
    uint16_t delayMs = map(twinkleParams.speed, 0, 255, 50, 5);
    
    if (millis() - lastUpdate > delayMs) {
        // Randomly light up new LEDs
        if (random8() < twinkleParams.intensity) {
            uint16_t idx = random16(NUM_LEDS);
            if (twinkleState[idx] == 0) {
                twinkleState[idx] = 1;  // Brightening
                twinkleBrightness[idx] = 0;
                
                switch (twinkleParams.colorMode) {
                    case TWINKLE_SINGLE:
                        twinkleColors[idx] = twinkleParams.twinkleColor;
                        break;
                    case TWINKLE_PALETTE:
                        twinkleColors[idx] = ColorFromPalette(pal, random8(), 255, LINEARBLEND);
                        break;
                    case TWINKLE_RANDOM:
                        twinkleColors[idx] = CHSV(random8(), 255, 255);
                        break;
                }
            }
        }
        
        // Update brightness
        uint8_t fadeStep = map(twinkleParams.fadeSpeed, 0, 255, 5, 30);
        
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            if (twinkleState[i] == 1) {
                // Brightening
                twinkleBrightness[i] = qadd8(twinkleBrightness[i], fadeStep * 2);
                if (twinkleBrightness[i] >= 250) {
                    twinkleState[i] = 2;  // Switch to dimming
                }
            } else if (twinkleState[i] == 2) {
                // Dimming
                twinkleBrightness[i] = qsub8(twinkleBrightness[i], fadeStep);
                if (twinkleBrightness[i] <= 5) {
                    twinkleState[i] = 0;
                    twinkleBrightness[i] = 0;
                }
            }
        }
        
        lastUpdate = millis();
    }
    
    // Render
    FastLED.clear();
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        if (twinkleBrightness[i] > 0) {
            CRGB col = twinkleColors[i];
            col.nscale8(twinkleBrightness[i]);
            leds[i] = col;
        }
    }
}

void effectTwinkleFox() {
    static uint8_t foxBrightness[NUM_LEDS];
    static CRGB foxColors[NUM_LEDS];
    static uint32_t lastUpdate = 0;
    
    CRGBPalette16 pal = getPalette(twinkleFoxParams.palette);
    
    uint16_t delayMs = map(twinkleFoxParams.speed, 0, 255, 30, 5);
    
    if (millis() - lastUpdate > delayMs) {
        // Randomly light up
        if (random8() < twinkleFoxParams.twinkleRate) {
            uint16_t idx = random16(NUM_LEDS);
            foxBrightness[idx] = 255;
            foxColors[idx] = ColorFromPalette(pal, random8(), 255, LINEARBLEND);
        }
        
        // Slowly fade all
        uint8_t fadeAmount = map(twinkleFoxParams.fadeOut, 0, 255, 1, 15);
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            foxBrightness[i] = qsub8(foxBrightness[i], fadeAmount);
        }
        
        lastUpdate = millis();
    }
    
    // Render
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        CRGB col = foxColors[i];
        col.nscale8(foxBrightness[i]);
        leds[i] = col;
    }
}

void effectSparkle() {
    static uint32_t lastSpark = 0;
    
    // Background
    if (!sparkleParams.overlay) {
        fill_solid(leds, NUM_LEDS, sparkleParams.colorBg);
    } else {
        // In overlay mode always fade sparkles
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            leds[i] = blend(leds[i], sparkleParams.colorBg, 30);
        }
    }
    
    uint16_t delayMs = map(sparkleParams.speed, 0, 255, 80, 10);
    
    if (millis() - lastSpark > delayMs) {
        // Random sparkles
        uint8_t numSparks = map(sparkleParams.intensity, 0, 255, 1, 10);
        for (uint8_t s = 0; s < numSparks; s++) {
            uint16_t idx = random16(NUM_LEDS);
            if (sparkleParams.darkMode) {
                leds[idx] = CRGB::Black;
            } else {
                leds[idx] = sparkleParams.colorSpark;
            }
        }
        lastSpark = millis();
    }
}

void effectGlitter() {
    static uint8_t hue = 0;
    
    if (!glitterParams.overlay) {
        // Without overlay: normal background (immediate)
        if (glitterParams.rainbowBg) {
            fill_rainbow(leds, NUM_LEDS, hue++, 7);
        } else {
            fill_solid(leds, NUM_LEDS, glitterParams.bgColor);
        }
    } else {
        // With overlay: smooth transition to background (glitter fades slower)
        if (glitterParams.rainbowBg) {
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                CRGB rainbowColor = CHSV(hue + (i * 7), 255, 255);
                leds[i] = blend(leds[i], rainbowColor, 30);
            }
            hue++;
        } else {
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                leds[i] = blend(leds[i], glitterParams.bgColor, 30);
            }
        }
    }
    
    // Add glitter
    uint8_t numGlitter = map(glitterParams.intensity, 0, 255, 1, 15);
    for (uint8_t g = 0; g < numGlitter; g++) {
        if (random8() < 80) {
            leds[random16(NUM_LEDS)] += CRGB::White;
        }
    }
}

void effectStarryNight() {
    static uint8_t starBrightness[NUM_LEDS];
    static int16_t shootingPos = -1;
    static uint32_t lastUpdate = 0;
    static uint32_t lastShoot = 0;
    
    uint16_t delayMs = map(starryNightParams.speed, 0, 255, 200, 5);
    
    if (millis() - lastUpdate > delayMs) {
        // Star twinkling
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            if (starBrightness[i] > 0) {
                // Random brightness fluctuations
                int8_t change = random8(20) - 10;
                starBrightness[i] = constrain((int16_t)starBrightness[i] + change, 0, 255);
                
                // Sometimes fades out
                if (random8() < 5) {
                    starBrightness[i] = qsub8(starBrightness[i], 30);
                }
            } else {
                // Randomly light up new stars (density 0-255 -> chance 0-25)
                uint8_t chance = map(starryNightParams.density, 0, 255, 1, 25);
                if (random8() < chance) {
                    starBrightness[i] = random8(100, 255);
                }
            }
        }
        lastUpdate = millis();
    }
    
    // Shooting star
    if (starryNightParams.shootingStars) {
        if (shootingPos < 0 && millis() - lastShoot > 3000 + random16(5000)) {
            shootingPos = 0;
            lastShoot = millis();
        }
        
        if (shootingPos >= 0) {
            shootingPos += 3;
            if (shootingPos >= NUM_LEDS) {
                shootingPos = -1;
            }
        }
    }
    
    // Render
    FastLED.clear();
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        if (starBrightness[i] > 0) {
            CRGB col = starryNightParams.colorStars;
            col.nscale8(starBrightness[i]);
            leds[i] = col;
        }
    }
    
    // Draw shooting star
    if (shootingPos >= 0) {
        for (int8_t t = 0; t < 8; t++) {
            int16_t pos = shootingPos - t;
            if (pos >= 0 && pos < NUM_LEDS) {
                uint8_t bright = 255 - t * 30;
                leds[pos] = CRGB(bright, bright, bright);
            }
        }
    }
}

// ============================================================================
// CATEGORY 5: FIRE/ORGANIC EFFECTS
// ============================================================================

// Buffer for fire effect
static uint8_t heat[NUM_LEDS];

void effectFire() {
    CRGBPalette16 pal = getPalette(fireParams.palette);
    
    // Cooling
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8(heat[i], random8(0, ((fireParams.cooling * 10) / NUM_LEDS) + 2));
    }
    
    // Move heat upwards
    for (uint16_t k = NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }
    
    // Random sparks at bottom
    if (random8() < fireParams.sparking) {
        uint8_t y = random8(7);
        if (y < NUM_LEDS) {
            heat[y] = qadd8(heat[y], random8(160, 255));
        }
    }
    
    // Boost
    if (fireParams.boost) {
        for (uint16_t i = 0; i < 3 && i < NUM_LEDS; i++) {
            heat[i] = qadd8(heat[i], 50);
        }
    }
    
    // Map to colors
    for (uint16_t j = 0; j < NUM_LEDS; j++) {
        uint8_t colorIndex = scale8(heat[j], 240);
        leds[j] = ColorFromPalette(pal, colorIndex, 255, LINEARBLEND);
    }
}

void effectCandle() {
    static uint8_t candleBrightness[NUM_LEDS];
    static uint32_t lastFlicker = 0;
    
    uint16_t delayMs = map(candleParams.speed, 0, 255, 80, 5);
    
    if (millis() - lastFlicker > delayMs) {
        // Intensity controls the RANGE of brightness fluctuations
        // 0 = almost no fluctuations (±5), 255 = dramatic fluctuations (±127)
        uint8_t flickerRange = map(candleParams.intensity, 0, 255, 5, 127);
        // Minimal brightness also depends on intensity
        uint8_t minBright = map(candleParams.intensity, 0, 255, 200, 20);
        
        if (candleParams.multiMode) {
            // Each LED as its own candle
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                int16_t change = random8(flickerRange * 2) - flickerRange;
                candleBrightness[i] = constrain((int16_t)candleBrightness[i] + change, minBright, 255);
            }
        } else {
            // All as one candle
            int16_t change = random8(flickerRange * 2) - flickerRange;
            uint8_t newBright = constrain((int16_t)candleBrightness[0] + change, minBright, 255);
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                candleBrightness[i] = newBright;
            }
        }
        lastFlicker = millis();
    }
    
    // Render
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        CRGB col = candleParams.color;
        
        // Color shift - changes color for each LED stably (not random)
        if (candleParams.colorShift > 0) {
            // Use sin8 with LED index for stable variation
            uint8_t variation = sin8(i * 17 + candleBrightness[i]);
            int16_t shiftAmount = map(variation, 0, 255, -candleParams.colorShift, candleParams.colorShift);
            
            // Shift mainly towards red/orange for fire effect
            col.r = constrain((int16_t)col.r + shiftAmount, 0, 255);
            col.g = constrain((int16_t)col.g + shiftAmount / 3, 0, 255);
        }
        
        col.nscale8(candleBrightness[i]);
        leds[i] = col;
    }
}

void effectFireFlicker() {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint8_t flicker = random8(fireFlickerParams.intensity);
        CRGB col = fireFlickerParams.color;
        col.nscale8(255 - flicker);
        leds[i] = col;
    }
    
    FastLED.delay(map(fireFlickerParams.speed, 0, 255, 100, 20));
}

void effectLava() {
    static uint16_t offset = 0;
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        // Two noise layers for blob effect
        uint8_t noise1 = inoise8(i * lavaParams.blobSize, offset);
        uint8_t noise2 = inoise8(i * lavaParams.blobSize + 1000, offset + 5000);
        
        uint8_t combined = (noise1 + noise2) / 2;
        
        // Map to colors
        CRGB col;
        if (combined < 128) {
            col = blend(CRGB::Black, CRGB::DarkRed, combined * 2);
        } else {
            col = blend(CRGB::DarkRed, CRGB::Yellow, (combined - 128) * 2);
        }
        
        // Smoothing - higher value = smoother transitions (min 10 to prevent animation freezing)
        uint8_t blendAmount = map(lavaParams.smoothness, 0, 255, 255, 30);
        leds[i] = blend(leds[i], col, blendAmount);
    }
    
    offset += map(lavaParams.speed, 0, 255, 5, 30);
}

void effectAurora() {
    static uint16_t offset = 0;
    CRGBPalette16 pal = getPalette(auroraParams.palette);
    
    // Intensity = wave size (low = thin, high = wide)
    uint8_t waveScale = map(auroraParams.intensity, 0, 255, 30, 8);
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint8_t noise = inoise8(i * waveScale, offset);
        uint8_t colorIdx = noise + (offset >> 4);
        uint8_t brightness = map(noise, 0, 255, 100, 255);
        
        leds[i] = ColorFromPalette(pal, colorIdx, brightness, LINEARBLEND);
    }
    
    offset += map(auroraParams.speed, 0, 255, 3, 30);
}

void effectPacifica() {
    // Simple ocean effect - color waves from palette
    static uint16_t offset = 0;
    CRGBPalette16 pal = getPalette(pacificaParams.palette);
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        // Three overlapping waves with different frequencies
        uint8_t wave1 = sin8(i * 7 + offset);
        uint8_t wave2 = sin8(i * 11 - offset / 2);
        uint8_t wave3 = sin8(i * 5 + offset / 3);
        
        // Combine waves
        uint8_t combined = (wave1 + wave2 + wave3) / 3;
        
        // Use combination as color index from palette
        uint8_t colorIdx = combined + (offset >> 3);
        
        // Brightness based on wave
        uint8_t brightness = map(combined, 0, 255, 120, 255);
        
        leds[i] = ColorFromPalette(pal, colorIdx, brightness, LINEARBLEND);
    }
    
    offset += map(pacificaParams.speed, 0, 255, 1, 15);
}

void effectLake() {
    static uint16_t offset = 0;
    CRGBPalette16 pal = getPalette(lakeParams.palette);
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        // Slow, calm rippling
        uint8_t wave1 = sin8(i * 5 + offset / 3);
        uint8_t wave2 = sin8(i * 7 - offset / 2);
        uint8_t combined = (wave1 + wave2) / 2;
        
        uint8_t colorIdx = i * 256 / NUM_LEDS + offset / 10;
        leds[i] = ColorFromPalette(pal, colorIdx, combined, LINEARBLEND);
    }
    
    offset += map(lakeParams.speed, 0, 255, 2, 15);
}

// ============================================================================
// CATEGORY 6: HOLIDAY EFFECTS
// ============================================================================

void effectFairy() {
    static uint8_t flasherBrightness[NUM_LEDS];
    static uint8_t flasherHue[NUM_LEDS];
    static uint8_t flasherState[NUM_LEDS];
    static uint32_t lastUpdate = 0;
    static bool initialized = false;
    
    // Normalize numFlashers: slider 1-255 -> 1-NUM_LEDS
    uint8_t numFlashers = map(fairyParams.numFlashers, 1, 255, 1, NUM_LEDS);
    if (numFlashers < 1) numFlashers = 1;
    if (numFlashers > NUM_LEDS) numFlashers = NUM_LEDS;
    
    if (!initialized) {
        for (uint8_t i = 0; i < NUM_LEDS; i++) {
            flasherBrightness[i] = random8(50, 200);
            flasherHue[i] = random8();
            flasherState[i] = random8(3);
        }
        initialized = true;
    }
    
    uint16_t delayMs = map(fairyParams.speed, 0, 255, 60, 8);
    
    if (millis() - lastUpdate > delayMs) {
        for (uint8_t i = 0; i < numFlashers; i++) {
            switch (flasherState[i]) {
                case 0: // Off
                    if (random8() < 25) flasherState[i] = 1;
                    break;
                case 1: // Brightening
                    flasherBrightness[i] = qadd8(flasherBrightness[i], 20);
                    if (flasherBrightness[i] >= 250) flasherState[i] = 2;
                    break;
                case 2: // Dimming
                    flasherBrightness[i] = qsub8(flasherBrightness[i], 10);
                    if (flasherBrightness[i] <= 80) {
                        flasherState[i] = 0;
                        flasherHue[i] = random8(); // New color
                    }
                    break;
            }
        }
        lastUpdate = millis();
    }
    
    // Black background
    FastLED.clear();
    
    // Distribute lights evenly
    uint16_t spacing = NUM_LEDS / max((uint8_t)1, numFlashers);
    
    for (uint8_t i = 0; i < numFlashers; i++) {
        uint16_t pos = (i * spacing + i * 7) % NUM_LEDS;
        
        CRGB col;
        // colorMode: 0=warm white, 1=cold white, 2=multicolor, 3=palette
        switch (fairyParams.colorMode) {
            case 0: // Warm white
                col = CRGB(255, 180, 100);
                break;
            case 1: // Cold white
                col = CRGB(200, 220, 255);
                break;
            case 2: // Multicolor
                col = CHSV(flasherHue[i], 255, 255);
                break;
            case 3: // Palette
            default:
                CRGBPalette16 pal = getPalette(fairyParams.palette);
                col = ColorFromPalette(pal, flasherHue[i], 255, LINEARBLEND);
                break;
        }
        
        col.nscale8(flasherBrightness[i]);
        leds[pos] = col;
    }
}

void effectChristmasChase() {
    static uint16_t offset = 0;
    static uint32_t lastStep = 0;
    static uint8_t sparkleBrightness[NUM_LEDS]; // Sparkle brightness for XMAS_SPARKLE
    static uint32_t lastSparkle = 0;
    
    uint16_t delayMs = map(christmasChaseParams.speed, 0, 255, 100, 15);
    
    if (millis() - lastStep > delayMs) {
        offset++;
        lastStep = millis();
    }
    
    switch (christmasChaseParams.pattern) {
        case XMAS_ALTERNATING:
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                if ((i + offset) % 6 < 3) {
                    leds[i] = christmasChaseParams.color1;
                } else {
                    leds[i] = christmasChaseParams.color2;
                }
            }
            break;
            
        case XMAS_CHASE:
            FastLED.clear();
            for (uint16_t i = 0; i < NUM_LEDS; i += 6) {
                uint16_t pos = (i + offset) % NUM_LEDS;
                leds[pos] = christmasChaseParams.color1;
                if (pos + 1 < NUM_LEDS) leds[pos + 1] = christmasChaseParams.color2;
            }
            break;
            
        case XMAS_SPARKLE:
            // Alternating background
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                leds[i] = (i % 2) ? christmasChaseParams.color1 : christmasChaseParams.color2;
            }
            
            // Fade out existing sparks - fade speed depends on speed
            uint8_t fadeAmount = map(christmasChaseParams.speed, 0, 255, 5, 30);
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                if (sparkleBrightness[i] > fadeAmount) {
                    sparkleBrightness[i] -= fadeAmount;
                } else {
                    sparkleBrightness[i] = 0;
                }
            }
            
            // Add new sparks according to speed
            if (millis() - lastSparkle > delayMs) {
                for (uint8_t s = 0; s < 5; s++) {
                    if (random8() < 80) {
                        sparkleBrightness[random16(NUM_LEDS)] = 255;
                    }
                }
                lastSparkle = millis();
            }
            
            // Overlay sparks on background
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                if (sparkleBrightness[i] > 0) {
                    CRGB sparkle = CRGB::White;
                    sparkle.nscale8(sparkleBrightness[i]);
                    leds[i] = blend(leds[i], CRGB::White, sparkleBrightness[i]);
                }
            }
            break;
    }
}

void effectHalloweenEyes() {
    static int16_t eyePositions[4] = {-1, -1, -1, -1};  // Eye pairs
    static uint8_t eyeBrightness[4] = {0};
    static uint8_t eyeState[4] = {0};  // 0=inactive, 1=appearing, 2=blinking, 3=fading
    static uint32_t eyeTimers[4] = {0};
    static uint32_t lastUpdate = 0;
    
    if (millis() - lastUpdate > 30) {
        // Manage eye pairs
        for (uint8_t e = 0; e < 2; e++) {
            switch (eyeState[e]) {
                case 0:  // Inactive - randomly activate
                    if (random8() < 20) {
                        eyePositions[e] = random8(NUM_LEDS - 5);
                        eyeState[e] = 1;
                        eyeBrightness[e] = 0;
                    }
                    break;
                    
                case 1:  // Appearing
                    eyeBrightness[e] = qadd8(eyeBrightness[e], 10);
                    if (eyeBrightness[e] >= 250) {
                        eyeState[e] = 2;
                        eyeTimers[e] = millis();
                    }
                    break;
                    
                case 2:  // Visible/Blinking
                    // Sometimes blink
                    if (random8() < 5) {
                        eyeBrightness[e] = random8(50, 200);
                    } else {
                        eyeBrightness[e] = 255;
                    }
                    
                    // After time start fading
                    if (millis() - eyeTimers[e] > halloweenEyesParams.duration) {
                        eyeState[e] = 3;
                    }
                    break;
                    
                case 3:  // Fading
                    // fadeTime controls fade speed (higher fadeTime = slower fade)
                    {
                        uint8_t fadeStep = map(halloweenEyesParams.fadeTime, 50, 1275, 20, 2);
                        eyeBrightness[e] = qsub8(eyeBrightness[e], fadeStep);
                    }
                    if (eyeBrightness[e] <= 5) {
                        eyeState[e] = 0;
                        eyePositions[e] = -1;
                    }
                    break;
            }
        }
        lastUpdate = millis();
    }
    
    // Render
    if (!halloweenEyesParams.overlay) {
        FastLED.clear();
    }
    
    for (uint8_t e = 0; e < 2; e++) {
        if (eyePositions[e] >= 0 && eyeBrightness[e] > 0) {
            CRGB col = halloweenEyesParams.color;
            col.nscale8(eyeBrightness[e]);
            
            // Left eye
            if (eyePositions[e] < NUM_LEDS) {
                leds[eyePositions[e]] = col;
            }
            // Right eye (2-4 LEDs further)
            uint16_t rightEye = eyePositions[e] + 3;
            if (rightEye < NUM_LEDS) {
                leds[rightEye] = col;
            }
        }
    }
}

// Structure for firework fragment
struct FireworkFragment {
    int16_t position;
    int8_t velocity;
    uint8_t brightness;
    CRGB color;
    bool active;
};

static FireworkFragment fragments[32];  // Max fragments

void effectFireworks() {
    static uint32_t lastLaunch = 0;
    static uint32_t lastUpdate = 0;
    
    // Normalize gravity: 0-255 -> 1-8 (visible effect on falling)
    uint8_t gravityForce = map(fireworksParams.gravity, 0, 255, 1, 8);
    
    if (millis() - lastUpdate > 20) {
        // Randomly launch new firework
        if (random8() < fireworksParams.chance / 4) {
            // Find free fragments
            int16_t launchPos = random8(NUM_LEDS);
            CRGB launchColor = CHSV(random8(), 255, 255);
            
            // Normalize fragments: 4-16 -> use directly
            uint8_t targetFragments = constrain(fireworksParams.fragments, 4, 16);
            
            uint8_t fragCount = 0;
            for (uint8_t f = 0; f < 32 && fragCount < targetFragments; f++) {
                if (!fragments[f].active) {
                    fragments[f].active = true;
                    fragments[f].position = launchPos * 10;  // Fixed point
                    fragments[f].velocity = random8(10, 30) * (random8(2) ? 1 : -1);
                    fragments[f].brightness = 255;
                    fragments[f].color = launchColor;
                    fragCount++;
                }
            }
            lastLaunch = millis();
        }
        
        // Update fragments
        for (uint8_t f = 0; f < 32; f++) {
            if (fragments[f].active) {
                fragments[f].position += fragments[f].velocity;
                
                // Gravity - now with visible effect
                fragments[f].velocity -= gravityForce;
                
                // Fading
                fragments[f].brightness = qsub8(fragments[f].brightness, 8);
                
                // Deactivation
                if (fragments[f].brightness < 10 || 
                    fragments[f].position < 0 || 
                    fragments[f].position >= NUM_LEDS * 10) {
                    fragments[f].active = false;
                }
            }
        }
        
        lastUpdate = millis();
    }
    
    // Render
    if (!fireworksParams.overlay) {
        fadeAll(50);  // Normal fading
    } else {
        fadeAll(10);  // Gentle fading in overlay mode to prevent saturation
    }
    
    for (uint8_t f = 0; f < 32; f++) {
        if (fragments[f].active) {
            int16_t ledPos = fragments[f].position / 10;
            if (ledPos >= 0 && ledPos < NUM_LEDS) {
                CRGB col = fragments[f].color;
                col.nscale8(fragments[f].brightness);
                // Use blend instead of += to avoid cumulation to white
                leds[ledPos] = blend(leds[ledPos], col, 180);
            }
        }
    }
}

void effectSnowSparkle() {
    static uint8_t snowBrightness[NUM_LEDS];
    static uint32_t lastUpdate = 0;
    static uint32_t lastSpawn = 0;
    
    uint16_t moveDelayMs = map(snowSparkleParams.speed, 0, 255, 80, 15);  // Movement speed
    uint16_t spawnDelayMs = map(snowSparkleParams.density, 0, 255, 500, 30);  // Frequency of new flakes
    
    if (snowSparkleParams.direction == DIR_FORWARD) {
        // Falling mode
        
        // Move flakes downward
        if (millis() - lastUpdate > moveDelayMs) {
            for (int16_t i = NUM_LEDS - 1; i > 0; i--) {
                snowBrightness[i] = snowBrightness[i - 1];
            }
            snowBrightness[0] = 0;  // Clear top
            lastUpdate = millis();
        }
        
        // Add new flakes at top
        if (millis() - lastSpawn > spawnDelayMs) {
            // Add flake in random position near top (0-2)
            uint8_t startPos = random8(3);
            if (startPos < NUM_LEDS) {
                snowBrightness[startPos] = 255;
            }
            lastSpawn = millis();
        }
        
    } else {
        // Random mode
        if (millis() - lastUpdate > moveDelayMs) {
            // New random flakes - add several at once depending on density
            uint8_t numSpawns = map(snowSparkleParams.density, 0, 255, 1, 5);
            for (uint8_t s = 0; s < numSpawns; s++) {
                if (random8() < 120) {  // High chance
                    snowBrightness[random16(NUM_LEDS)] = 255;
                }
            }
            
            // Fade out in random mode - slower fade
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                snowBrightness[i] = qsub8(snowBrightness[i], 8);
            }
            
            lastUpdate = millis();
        }
    }
    
    // Render
    FastLED.clear();
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        if (snowBrightness[i] > 0) {
            CRGB col = snowSparkleParams.color;
            col.nscale8(snowBrightness[i]);
            leds[i] = col;
        }
    }
}

// ============================================================================
// CATEGORY 7: SPECIAL EFFECTS
// ============================================================================

// Structure for ball
struct Ball {
    float position;
    float velocity;
    float height;
    CRGB color;
};

static Ball balls[8];

void effectBouncingBalls() {
    static bool initialized = false;
    static uint32_t lastUpdate = 0;
    static uint8_t lastNumBalls = 0;
    
    CRGBPalette16 pal = getPalette(bouncingBallsParams.palette);
    
    // Reinitialize when number of balls changes or on first run
    if (!initialized || lastNumBalls != bouncingBallsParams.numBalls) {
        for (uint8_t i = 0; i < 8; i++) {
            // Distribute balls at different starting positions
            balls[i].position = (i * NUM_LEDS / 8);
            balls[i].velocity = 0;
            balls[i].height = random8(NUM_LEDS / 2, NUM_LEDS);
        }
        lastNumBalls = bouncingBallsParams.numBalls;
        initialized = true;
    }
    
    float gravity = (float)bouncingBallsParams.gravity / 5000.0;
    float damping = 0.9;
    
    if (millis() - lastUpdate > 15) {
        for (uint8_t i = 0; i < bouncingBallsParams.numBalls && i < 8; i++) {
            balls[i].velocity += gravity;
            balls[i].position += balls[i].velocity;
            
            // Bounce from bottom
            if (balls[i].position >= NUM_LEDS - 1) {
                balls[i].position = NUM_LEDS - 1;
                balls[i].velocity = -balls[i].velocity * damping;
                
                // Reset if too slow
                if (abs(balls[i].velocity) < 0.5) {
                    balls[i].position = 0;
                    balls[i].velocity = 0;
                }
            }
            
            // Bounce from top
            if (balls[i].position < 0) {
                balls[i].position = 0;
                balls[i].velocity = -balls[i].velocity * damping;
            }
        }
        lastUpdate = millis();
    }
    
    // Render - use only trail to control fading
    fadeAll(bouncingBallsParams.trail > 0 ? 50 : 255);
    
    for (uint8_t i = 0; i < bouncingBallsParams.numBalls && i < 8; i++) {
        int16_t pos = (int16_t)balls[i].position;
        // Get color from palette dynamically - responds to palette change
        CRGB ballColor = ColorFromPalette(pal, i * 32, 255, LINEARBLEND);
        
        if (pos >= 0 && pos < NUM_LEDS) {
            leds[pos] = ballColor;
            
            // Trail
            if (bouncingBallsParams.trail > 0) {
                for (uint8_t t = 1; t <= bouncingBallsParams.trail; t++) {
                    int16_t trailPos = pos - (balls[i].velocity > 0 ? t : -t);
                    if (trailPos >= 0 && trailPos < NUM_LEDS) {
                        CRGB col = ballColor;
                        col.nscale8(255 - t * (255 / bouncingBallsParams.trail));
                        // Use blend instead of += to avoid saturation
                        leds[trailPos] = blend(leds[trailPos], col, 180);
                    }
                }
            }
        }
    }
}

struct PopcornKernel {
    float position;
    float velocity;
    CRGB color;
    bool active;
};

static PopcornKernel kernels[20];

void effectPopcorn() {
    static uint32_t lastUpdate = 0;
    static uint32_t lastPop = 0;
    
    CRGBPalette16 pal = getPalette(popcornParams.palette);
    
    // Speed controls physics update tempo
    uint16_t updateDelay = map(popcornParams.speed, 0, 255, 40, 10);
    // Intensity controls frequency of new kernels
    uint16_t popDelay = map(popcornParams.intensity, 0, 255, 800, 50);
    
    // Adding new kernels
    if (millis() - lastPop > popDelay) {
        for (uint8_t k = 0; k < 20; k++) {
            if (!kernels[k].active) {
                kernels[k].active = true;
                // Kernels start from random position near bottom (simulating pan frying)
                kernels[k].position = random8(5);
                // Different jump heights - most small/medium, but sometimes "super" jump
                if (random8() < 20) {
                    // ~8% chance for super jump - flies to the very top
                    kernels[k].velocity = (float)random8(90, 120) / 10.0;  // 9.0 - 12.0
                } else {
                    // Normal jump
                    kernels[k].velocity = (float)random8(20, 80) / 10.0;   // 2.0 - 8.0
                }
                // Dynamic color from palette
                kernels[k].color = ColorFromPalette(pal, random8(), 255, LINEARBLEND);
                break;
            }
        }
        lastPop = millis();
    }
    
    // Physics update
    if (millis() - lastUpdate > updateDelay) {
        for (uint8_t k = 0; k < 20; k++) {
            if (kernels[k].active) {
                // Gravity
                kernels[k].velocity -= 0.25;
                kernels[k].position += kernels[k].velocity;
                
                // Bounce from ground with damping (simulating bouncing)
                if (kernels[k].position < 0) {
                    kernels[k].position = 0;
                    kernels[k].velocity = -kernels[k].velocity * 0.6;  // Bounce with energy loss
                    
                    // Deactivate if too little energy
                    if (abs(kernels[k].velocity) < 0.3) {
                        kernels[k].active = false;
                    }
                }
                
                // Deactivate if flew too high
                if (kernels[k].position >= NUM_LEDS) {
                    kernels[k].active = false;
                }
            }
        }
        lastUpdate = millis();
    }
    
    // Render
    fadeAll(80);
    
    for (uint8_t k = 0; k < 20; k++) {
        if (kernels[k].active) {
            int16_t pos = (int16_t)kernels[k].position;
            if (pos >= 0 && pos < NUM_LEDS) {
                leds[pos] = kernels[k].color;
            }
        }
    }
}

struct Drip {
    float position;
    float velocity;
    bool active;
};

static Drip drips[8];

void effectDrip() {
    static uint32_t lastUpdate = 0;
    static uint8_t dripState[8] = {0};      // 0=ready, 1=falling, 2=splashing
    static uint8_t splashBrightness[8] = {0};
    static uint32_t nextDripTime = 0;
    
    float gravity = (float)dripParams.gravity / 2500.0;
    
    if (millis() - lastUpdate > 20) {
        // Try to add new drip - only if time has passed
        if (millis() > nextDripTime) {
            for (uint8_t d = 0; d < dripParams.numDrips && d < 8; d++) {
                if (dripState[d] == 0) {  // Ready for new drip
                    dripState[d] = 1;
                    drips[d].active = true;
                    drips[d].position = 0;
                    drips[d].velocity = 0.2;
                    // Next drip after 800-1500ms
                    nextDripTime = millis() + 800 + random16(700);
                    break;
                }
            }
        }
        
        // Update all drips
        for (uint8_t d = 0; d < 8; d++) {
            if (dripState[d] == 1) {
                // Falling
                drips[d].velocity += gravity;
                drips[d].position += drips[d].velocity;
                
                // Reached bottom - splash!
                if (drips[d].position >= NUM_LEDS - 1) {
                    dripState[d] = 2;
                    splashBrightness[d] = 255;
                    drips[d].active = false;
                }
            } else if (dripState[d] == 2) {
                // Splash fades
                splashBrightness[d] = qsub8(splashBrightness[d], 12);
                if (splashBrightness[d] < 5) {
                    dripState[d] = 0;  // Ready for next drip
                }
            }
        }
        
        lastUpdate = millis();
    }
    
    // Render
    if (!dripParams.overlay) {
        fadeAll(30);
    } else {
        fadeAll(10);
    }
    
    for (uint8_t d = 0; d < 8; d++) {
        if (dripState[d] == 1 && drips[d].active) {
            // Falling drip
            int16_t pos = (int16_t)drips[d].position;
            
            if (pos >= 0 && pos < NUM_LEDS) {
                leds[pos] = dripParams.color;
            }
            
            // Tail
            uint8_t tailLen = constrain((int)(drips[d].velocity * 1.5), 1, 6);
            for (uint8_t t = 1; t <= tailLen; t++) {
                int16_t tailPos = pos - t;
                if (tailPos >= 0 && tailPos < NUM_LEDS) {
                    CRGB col = dripParams.color;
                    col.nscale8(255 - (t * 40));
                    leds[tailPos] = col;
                }
            }
        } else if (dripState[d] == 2) {
            // Splash at bottom
            CRGB splashCol = dripParams.color;
            splashCol.nscale8(splashBrightness[d]);
            
            // Main impact point
            leds[NUM_LEDS - 1] = splashCol;
            
            // Splash up (8 pixels)
            for (uint8_t s = 1; s <= 8; s++) {
                int16_t splashPos = NUM_LEDS - 1 - s;
                if (splashPos >= 0) {
                    CRGB col = dripParams.color;
                    col.nscale8(splashBrightness[d] * (9 - s) / 9);
                    leds[splashPos] = blend(leds[splashPos], col, splashBrightness[d]);
                }
            }
        }
    }
}

void effectPlasma() {
    static uint16_t phase1 = 0;
    static uint16_t phase2 = 0;
    
    // Intensity controls wave scale (1-20)
    uint8_t waveScale = map(plasmaParams.intensity, 0, 255, 3, 20);
    
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        uint8_t sin1 = sin8(i * waveScale + phase1);
        uint8_t sin2 = sin8(i * (waveScale + 5) - phase2);
        uint8_t sin3 = sin8(i * (waveScale / 2) + phase1 / 2);
        
        uint8_t colorIndex = (sin1 + sin2 + sin3) / 3;
        
        leds[i] = CHSV(colorIndex + plasmaParams.phase, 255, 255);
    }
    
    phase1 += map(plasmaParams.speed, 0, 255, 2, 15);
    phase2 += map(plasmaParams.speed, 0, 255, 3, 20);
}

void effectLightning() {
    static uint32_t lastFlash = 0;
    static uint8_t flashState = 0;
    static uint8_t flashCount = 0;
    static int16_t flashStart = 0;
    static int16_t flashLen = 0;
    
    // Frequency mapped: 0=rarely, 255=often
    uint8_t flashChance = map(lightningParams.frequency, 0, 255, 3, 80);
    
    // New flash
    if (flashState == 0 && random8() < flashChance) {
        flashState = 1;
        flashCount = random8(2, 5);  // 2-4 flashes in series
        flashStart = random8(NUM_LEDS / 4, NUM_LEDS * 3 / 4);  // Middle section
        flashLen = random8(8, 25);
    }
    
    // Stormy background
    CRGB bgColor = CRGB(20, 20, 50);  // Dark navy (survives low brightness)
    
    if (lightningParams.overlay) {
        // Overlay - fade first, then gently add background
        fadeAll(25);
        for (uint16_t i = 0; i < NUM_LEDS; i++) {
            leds[i] = blend(leds[i], bgColor, 30);  // Gentle blend with background
        }
    } else {
        // Normal mode - full background
        fill_solid(leds, NUM_LEDS, bgColor);
    }
    
    // Flash handling
    if (flashState > 0) {
        if (flashState == 1) {
            // FLASH! - white core
            for (int16_t i = flashStart; i < flashStart + flashLen && i < NUM_LEDS; i++) {
                if (i >= 0) {
                    CRGB core = CRGB::White;
                    core.nscale8(lightningParams.intensity);
                    leds[i] = core;
                }
            }
            
            // Colored glow on edges
            for (uint8_t edge = 1; edge <= 4; edge++) {
                int16_t leftPos = flashStart - edge;
                int16_t rightPos = flashStart + flashLen - 1 + edge;
                CRGB glow = lightningParams.color;
                glow.nscale8(lightningParams.intensity / (edge + 1));
                
                if (leftPos >= 0) leds[leftPos] = glow;
                if (rightPos < NUM_LEDS) leds[rightPos] = glow;
            }
            
            // Random branches
            for (uint8_t b = 0; b < 2; b++) {
                int16_t branchPos = flashStart + random8(flashLen + 4) - 2;
                if (branchPos >= 0 && branchPos < NUM_LEDS) {
                    leds[branchPos] = lightningParams.color;
                }
            }
            
            flashState = 2;
            lastFlash = millis();
        } else if (flashState == 2 && millis() - lastFlash > 40 + random8(60)) {
            // Pause between flashes
            flashCount--;
            if (flashCount > 0) {
                flashState = 1;
                flashStart += random8(5) - 2;
                flashLen = random8(6, 18);
            } else {
                flashState = 0;
            }
        }
    }
}

struct MatrixDrop {
    int16_t position;
    uint8_t speed;
    bool active;
};

static MatrixDrop matrixDrops[20];

void effectMatrix() {
    static uint32_t lastUpdate = 0;
    
    // Always use color from parameters
    CRGB dropColor = matrixParams.color;
    
    uint16_t delayMs = map(matrixParams.speed, 0, 255, 80, 15);
    
    if (millis() - lastUpdate > delayMs) {
        // spawningRate - minimum 10 to always have drops
        uint8_t spawnChance = max((uint8_t)10, matrixParams.spawningRate);
        
        // New drops
        if (random8() < spawnChance) {
            for (uint8_t d = 0; d < 20; d++) {
                if (!matrixDrops[d].active) {
                    matrixDrops[d].active = true;
                    matrixDrops[d].position = 0;
                    matrixDrops[d].speed = random8(1, 3);
                    break;
                }
            }
        }
        
        // Update drops
        for (uint8_t d = 0; d < 20; d++) {
            if (matrixDrops[d].active) {
                matrixDrops[d].position += matrixDrops[d].speed;
                
                if (matrixDrops[d].position >= NUM_LEDS + matrixParams.trailLength) {
                    matrixDrops[d].active = false;
                }
            }
        }
        
        lastUpdate = millis();
    }
    
    // Render
    FastLED.clear();
    
    for (uint8_t d = 0; d < 20; d++) {
        if (matrixDrops[d].active) {
            int16_t headPos = matrixDrops[d].position;
            
            // Head of drop (white/bright)
            if (headPos >= 0 && headPos < NUM_LEDS) {
                leds[headPos] = CRGB::White;
            }
            
            // Tail - trailLength now works clearly
            uint8_t actualTrail = constrain(matrixParams.trailLength, 3, 30);
            for (uint8_t t = 1; t <= actualTrail; t++) {
                int16_t tailPos = headPos - t;
                if (tailPos >= 0 && tailPos < NUM_LEDS) {
                    // Better gradient - exponential fade
                    uint8_t fadeAmount = 255 * (actualTrail - t + 1) / (actualTrail + 1);
                    CRGB col = dropColor;
                    col.nscale8(fadeAmount);
                    leds[tailPos] = col;
                }
            }
        }
    }
}

void effectHeartbeat() {
    static uint32_t lastBeat = 0;
    static uint8_t beatPhase = 0;  // 0=pause, 1=first, 2=pause2, 3=second
    static uint8_t brightness = 0;
    
    uint32_t beatInterval = 60000 / heartbeatParams.bpm;
    uint32_t now = millis();
    
    // Simulation of double heartbeat
    switch (beatPhase) {
        case 0:  // Pause before first beat
            if (now - lastBeat > beatInterval) {
                beatPhase = 1;
                lastBeat = now;
            }
            brightness = qsub8(brightness, 10);
            break;
            
        case 1:  // First beat (stronger)
            if (now - lastBeat < 80) {
                brightness = 255;  // Maximum brightness
            } else {
                beatPhase = 2;
                lastBeat = now;
            }
            break;
            
        case 2:  // Short pause
            brightness = qsub8(brightness, 30);
            if (now - lastBeat > 100) {
                beatPhase = 3;
                lastBeat = now;
            }
            break;
            
        case 3:  // Second beat (weaker)
            if (now - lastBeat < 60) {
                brightness = 192;  // 75% brightness
            } else {
                beatPhase = 0;
                lastBeat = now;
            }
            break;
    }
    
    // Fade outside beats
    if (beatPhase == 0 || beatPhase == 2) {
        brightness = qsub8(brightness, 15);
    }
    
    // Render
    CRGB col = heartbeatParams.color;
    col.nscale8(brightness);
    fill_solid(leds, NUM_LEDS, col);
}

// ============================================================================
// CATEGORY 8: BREATHING/FADE EFFECTS
// ============================================================================

void effectBreathe() {
    static uint16_t phase = 0;
    
    // Sinusoidal breathing
    uint8_t breath = sin8(phase);
    
    CRGB col;
    if (breatheParams.twoColor) {
        col = blend(breatheParams.colorPrimary, breatheParams.colorSecondary, breath);
    } else {
        col = breatheParams.colorPrimary;
        col.nscale8(breath);
    }
    
    fill_solid(leds, NUM_LEDS, col);
    
    phase += map(breatheParams.speed, 0, 255, 1, 8);
}

void effectDissolve() {
    static uint8_t pixelState[NUM_LEDS];  // 0=off, 1=on
    static uint8_t dissolvePhase = 0;      // 0=filling, 1=dissolving
    static uint16_t activeCount = 0;
    static uint32_t lastStep = 0;
    static CRGB currentColor;
    
    uint16_t delayMs = map(dissolveParams.repeatSpeed, 0, 255, 50, 10);
    
    if (millis() - lastStep > delayMs) {
        if (dissolvePhase == 0) {
            // Filling phase
            uint8_t toFill = map(dissolveParams.dissolveSpeed, 0, 255, 1, 5);
            for (uint8_t f = 0; f < toFill && activeCount < NUM_LEDS; f++) {
                // Find random unfilled pixel
                uint16_t attempts = 0;
                while (attempts < 50) {
                    uint16_t idx = random16(NUM_LEDS);
                    if (pixelState[idx] == 0) {
                        pixelState[idx] = 1;
                        activeCount++;
                        break;
                    }
                    attempts++;
                }
            }
            
            if (activeCount >= NUM_LEDS) {
                dissolvePhase = 1;
            }
        } else {
            // Dissolving phase
            uint8_t toDissolve = map(dissolveParams.dissolveSpeed, 0, 255, 1, 5);
            for (uint8_t d = 0; d < toDissolve && activeCount > 0; d++) {
                uint16_t attempts = 0;
                while (attempts < 50) {
                    uint16_t idx = random16(NUM_LEDS);
                    if (pixelState[idx] == 1) {
                        pixelState[idx] = 0;
                        activeCount--;
                        break;
                    }
                    attempts++;
                }
            }
            
            if (activeCount == 0) {
                dissolvePhase = 0;
                if (dissolveParams.randomColors) {
                    currentColor = CHSV(random8(), 255, 255);
                } else {
                    currentColor = dissolveParams.color;
                }
            }
        }
        
        lastStep = millis();
    }
    
    // Render
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        if (pixelState[i]) {
            leds[i] = currentColor;
        } else {
            leds[i] = CRGB::Black;
        }
    }
}

void effectFade() {
    static uint16_t phase = 0;
    static uint8_t currentColor = 0;
    
    uint8_t blendAmount = phase & 0xFF;
    uint8_t nextColor = currentColor + 1;
    bool isLastToFirst = false;
    
    // Check if we're transitioning from last to first
    if (nextColor >= fadeParams.numColors) {
        nextColor = 0;
        isLastToFirst = true;
    }
    
    CRGB col;
    
    // If loop is off and we're going from last to first, don't blend - just show current color
    if (!fadeParams.loop && isLastToFirst) {
        col = fadeParams.colors[currentColor];
    } else {
        col = blend(fadeParams.colors[currentColor], 
                    fadeParams.colors[nextColor], 
                    blendAmount);
    }
    
    fill_solid(leds, NUM_LEDS, col);
    
    phase += map(fadeParams.speed, 0, 255, 1, 8);
    
    if (phase >= 256) {
        phase = 0;
        currentColor = nextColor;
    }
}

// ============================================================================
// CATEGORY 9: ALARM EFFECTS
// ============================================================================

void effectPolice() {
    static uint32_t lastSwitch = 0;
    static bool side = false;
    static uint8_t flashCount = 0;
    
    uint16_t flashInterval = map(policeLightsParams.speed, 0, 255, 150, 30);
    
    if (millis() - lastSwitch > flashInterval) {
        flashCount++;
        if (flashCount >= 3) {
            flashCount = 0;
            side = !side;
        }
        lastSwitch = millis();
    }
    
    switch (policeLightsParams.style) {
        case POLICE_SINGLE:
            fill_solid(leds, NUM_LEDS, side ? policeLightsParams.color1 : policeLightsParams.color2);
            break;
            
        case POLICE_SOLID:
            if (flashCount % 2 == 0) {
                fill_solid(leds, NUM_LEDS, side ? policeLightsParams.color1 : policeLightsParams.color2);
            } else {
                FastLED.clear();
            }
            break;
            
        case POLICE_ALTERNATING:
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
                if (i < NUM_LEDS / 2) {
                    leds[i] = side ? policeLightsParams.color1 : CRGB::Black;
                } else {
                    leds[i] = side ? CRGB::Black : policeLightsParams.color2;
                }
            }
            // Flash effect
            if (flashCount % 2 == 1) {
                for (uint16_t i = 0; i < NUM_LEDS; i++) {
                    leds[i].nscale8(50);
                }
            }
            break;
    }
}

void effectStrobe() {
    static uint32_t lastFlash = 0;
    static bool on = false;
    static uint8_t hue = 0;
    static uint8_t megaFlashCount = 0;
    
    uint16_t interval = map(strobeParams.frequency, 0, 255, 200, 20);
    
    switch (strobeParams.mode) {
        case STROBE_NORMAL:
            // Single flash with chosen color
            if (millis() - lastFlash > (on ? 30 : interval)) {
                on = !on;
                lastFlash = millis();
            }
            if (on) {
                fill_solid(leds, NUM_LEDS, strobeParams.color);
            } else {
                FastLED.clear();
            }
            break;
            
        case STROBE_MEGA:
            // Rapid triple flashes - first 2 in color, 3rd in white
            {
                uint16_t megaInterval = interval / 2; // 2x faster base
                if (millis() - lastFlash > (on ? 15 : megaInterval)) {
                    on = !on;
                    lastFlash = millis();
                    if (!on) {
                        megaFlashCount++;
                        if (megaFlashCount >= 3) {
                            megaFlashCount = 0;
                            // Extra pause after burst
                            lastFlash = millis() - megaInterval + interval / 2;
                        }
                    }
                }
                if (on) {
                    // Flash 1 and 2 = chosen color, flash 3 = white
                    CRGB flashColor = (megaFlashCount < 2) ? strobeParams.color : CRGB::White;
                    fill_solid(leds, NUM_LEDS, flashColor);
                } else {
                    FastLED.clear();
                }
            }
            break;
            
        case STROBE_RAINBOW:
            // Rainbow color cycling strobe
            if (millis() - lastFlash > (on ? 25 : interval)) {
                on = !on;
                lastFlash = millis();
                if (on) {
                    hue += 15; // Change color each flash
                }
            }
            if (on) {
                fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
            } else {
                FastLED.clear();
            }
            break;
    }
}

#endif // EFFECTS_H
