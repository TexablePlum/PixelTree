/*
 * EffectDefs.h - Global effect definitions (variables & helper functions)
 * 
 * Must be included before Effects.h to provide global state
 */

#ifndef EFFECT_DEFS_H
#define EFFECT_DEFS_H

#include <FastLED.h>
#include "Config.h"
#include "EffectParams.h"
#include "Palettes.h"

// Define NUM_LEDS for compatibility with Effects.h 
// (Effects.h uses NUM_LEDS, Config.h uses ARGB_NUM_LEDS)
#ifndef NUM_LEDS
#define NUM_LEDS ARGB_NUM_LEDS
#endif

// ============================================================================
// Global LED Array
// ============================================================================

CRGB leds[ARGB_NUM_LEDS];

// ============================================================================
// Helper Functions (used by Effects.h)
// ============================================================================

// Fade all LEDs by a given amount
inline void fadeAll(uint8_t amount) {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        leds[i].nscale8(255 - amount);
    }
}

// Get color from palette
inline CRGB getColorFromPalette(PaletteType paletteType, uint8_t index, uint8_t brightness = 255) {
    CRGBPalette16 palette = getPalette(paletteType);
    return ColorFromPalette(palette, index, brightness, LINEARBLEND);
}

// ============================================================================
// Effect Parameter Instances (with defaults)
// ============================================================================

// Category 1: Static
SolidParams solidParams = { .color = CRGB::White, .brightness = 255 };

GradientParams gradientParams = {
    .colorStart = CRGB::Red, .colorMiddle = CRGB::Green, .colorEnd = CRGB::Blue,
    .style = GRADIENT_LINEAR, .threePoint = true
};

SpotsParams spotsParams = { .color = CRGB::Gold, .spread = 8, .width = 2, .fade = true };

PatternParams patternParams = { .colorFg = CRGB::Red, .colorBg = CRGB::Green, .fgSize = 3, .bgSize = 2 };

// Category 2: Wave
RainbowWaveParams rainbowWaveParams = { .speed = 120, .size = 15, .direction = DIR_FORWARD, .saturation = 255 };

ColorWaveParams colorWaveParams = {
    .colors = {
        CRGB::Purple,   // color1
        CRGB::Cyan,     // color2
        CRGB::Yellow,   // color3
        CRGB::Magenta,  // color4
        CRGB::Orange,   // color5
        CRGB::Lime,     // color6
        CRGB::Pink,     // color7
        CRGB::Aqua      // color8
    },
    .numColors = 4, .speed = 100, .direction = DIR_FORWARD
};

OscillateParams oscillateParams = { .colorPrimary = CRGB::Blue, .colorSecondary = CRGB::Orange, .speed = 80, .pointSize = 5 };

WavyParams wavyParams = { .palette = PALETTE_OCEAN, .speed = 100, .amplitude = 128, .frequency = 3 };

// Category 3: Chase
TheaterChaseParams theaterChaseParams = { .color = CRGB::Red, .speed = 100, .gapSize = 3, .rainbowMode = false };

ScannerParams scannerParams = {
    .colors = {CRGB::Red, CRGB::Blue, CRGB::Green, CRGB::Yellow, 
               CRGB::Cyan, CRGB::Magenta, CRGB::Orange, CRGB::Purple},
    .speed = 80, .numDots = 1, .trailLength = 15,
    .dualMode = false, .overlay = false
};

CometParams cometParams = { 
    .color = CRGB::Cyan, .sparkleColor = CRGB::Gold, .speed = 120, 
    .trailLength = 20, .sparkleEnabled = true, .direction = DIR_FORWARD 
};

RunningLightsParams runningLightsParams = { 
    .colors = {CRGB::Green, CRGB::Blue, CRGB::Purple, CRGB::Orange}, 
    .numColors = 1, .speed = 100, .waveWidth = 10, .dualMode = false, .shape = SHAPE_SINE 
};

AndroidParams androidParams = { .colorPrimary = CRGB::Lime, .colorSecondary = CRGB::Black, .speed = 100, .sectionWidth = 20 };

// Category 4: Twinkle
TwinkleParams twinkleParams = { .twinkleColor = CRGB::White, .palette = PALETTE_RAINBOW, .speed = 150, .intensity = 80, .fadeSpeed = 100, .colorMode = TWINKLE_PALETTE };

TwinkleFoxParams twinkleFoxParams = { .palette = PALETTE_SNOW, .speed = 100, .twinkleRate = 100, .fadeOut = 200 };

SparkleParams sparkleParams = { .colorSpark = CRGB::White, .colorBg = CRGB::Blue, .speed = 180, .intensity = 50, .overlay = true, .darkMode = false };

GlitterParams glitterParams = { .intensity = 80, .rainbowBg = true, .bgColor = CRGB::Black, .overlay = true };

StarryNightParams starryNightParams = { .speed = 100, .density = 60, .colorStars = CRGB::White, .shootingStars = true };

// Category 5: Fire/Organic
FireParams fireParams = { .cooling = 55, .sparking = 120, .boost = false, .palette = PALETTE_HEAT };

CandleParams candleParams = { .speed = 100, .intensity = 150, .multiMode = true, .color = CRGB(255, 147, 41), .colorShift = 30 };

FireFlickerParams fireFlickerParams = { .speed = 120, .intensity = 200, .color = CRGB::OrangeRed };

LavaParams lavaParams = { .speed = 40, .blobSize = 20, .smoothness = 200 };

AuroraParams auroraParams = { .speed = 80, .intensity = 150, .palette = PALETTE_AURORA };

PacificaParams pacificaParams = { .speed = 100, .palette = PALETTE_OCEAN };

LakeParams lakeParams = { .speed = 60, .palette = PALETTE_FOREST };

// Category 6: Christmas/Seasonal
FairyParams fairyParams = { .speed = 150, .numFlashers = 15, .colorMode = FAIRY_WARM_WHITE, .palette = PALETTE_PARTY };

ChristmasChaseParams christmasChaseParams = { .speed = 100, .color1 = CRGB::Red, .color2 = CRGB::Green, .pattern = XMAS_CHASE };

HalloweenEyesParams halloweenEyesParams = { .duration = 2000, .fadeTime = 500, .color = CRGB::Red, .overlay = false };

FireworksParams fireworksParams = { .chance = 80, .fragments = 8, .gravity = 128, .overlay = false };

SnowSparkleParams snowSparkleParams = { .speed = 150, .density = 40, .color = CRGB::White, .direction = DIR_FORWARD };

// Category 7: Special
BouncingBallsParams bouncingBallsParams = { .gravity = 200, .numBalls = 3, .overlay = false, .trail = 5, .palette = PALETTE_RAINBOW };

PopcornParams popcornParams = { .speed = 150, .intensity = 100, .palette = PALETTE_PARTY };

DripParams dripParams = { .gravity = 180, .numDrips = 4, .overlay = false, .color = CRGB::Aqua };

PlasmaParams plasmaParams = { .phase = 0, .intensity = 200, .speed = 80 };

LightningParams lightningParams = { .frequency = 50, .intensity = 255, .color = CRGB::White, .overlay = false };

MatrixParams matrixParams = { .speed = 150, .spawningRate = 80, .trailLength = 10, .color = CRGB::Green };

HeartbeatParams heartbeatParams = { .bpm = 72, .color = CRGB::Red };

// Category 8: Breathing/Fade
BreatheParams breatheParams = { .speed = 60, .colorPrimary = CRGB::Purple, .colorSecondary = CRGB::Cyan, .twoColor = true };

DissolveParams dissolveParams = { .repeatSpeed = 100, .dissolveSpeed = 80, .randomColors = true, .color = CRGB::Magenta };

FadeParams fadeParams = { .speed = 80, .colors = {CRGB::Red, CRGB::Yellow, CRGB::Green, CRGB::Cyan, CRGB::Blue, CRGB::Magenta, CRGB::Orange, CRGB::Purple}, .numColors = 3, .loop = true };

// Category 9: Alarm
PoliceLightsParams policeLightsParams = { .speed = 150, .color1 = CRGB::Blue, .color2 = CRGB::Red, .style = POLICE_ALTERNATING };

StrobeParams strobeParams = { .frequency = 200, .color = CRGB::White, .mode = STROBE_NORMAL };

// ============================================================================
// Helper Functions
// ============================================================================

// Map LED position based on direction
uint16_t mapLed(uint16_t pos, Direction dir) {
    switch (dir) {
        case DIR_REVERSE: // Reverse/Left
        case DIR_DOWN:    // Down (same as reverse for 1D strip)
        case DIR_CCW:     // Counter-clockwise (same as reverse)
            return ARGB_NUM_LEDS - 1 - pos;
        
        case DIR_FORWARD: // Forward/Right (default)
        case DIR_UP:      // Up (same as forward for 1D strip)
        case DIR_CW:      // Clockwise (same as forward)
        default:
            return pos;
    }
}

// Safe LED set with bounds checking
void setLedSafe(uint16_t pos, CRGB color) {
    if (pos < ARGB_NUM_LEDS) {
        leds[pos] = color;
    }
}

// Add color (blend)
void addLedSafe(uint16_t pos, CRGB color) {
    if (pos < ARGB_NUM_LEDS) {
        leds[pos] += color;
    }
}

// Blend with fade
void blendLedSafe(uint16_t pos, CRGB color, uint8_t amount) {
    if (pos < ARGB_NUM_LEDS) {
        leds[pos] = blend(leds[pos], color, amount);
    }
}

#endif // EFFECT_DEFS_H
