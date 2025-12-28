/*
 * EffectParams.h - Effect Parameter Definitions
 * 
 * Each effect has its own parameter structure allowing
 * configuration without modifying the effect code.
 */

#ifndef EFFECT_PARAMS_H
#define EFFECT_PARAMS_H

#include <FastLED.h>

// ============== ENUMERATION TYPES ==============

// Animation direction
enum Direction {
    DIR_FORWARD = 0,
    DIR_REVERSE = 1,
    DIR_UP = 2,
    DIR_DOWN = 3,
    DIR_CW = 4,      // Clockwise
    DIR_CCW = 5      // Counter-clockwise
};

// Gradient style
enum GradientStyle {
    GRADIENT_LINEAR = 0,
    GRADIENT_MIRROR = 1,
    GRADIENT_SCATTERED = 2
};

// Wave shape
enum WaveShape {
    SHAPE_SINE = 0,
    SHAPE_SAW = 1,
    SHAPE_SQUARE = 2,
    SHAPE_TRIANGLE = 3
};

// Twinkle modes
enum TwinkleMode {
    TWINKLE_SINGLE = 0,
    TWINKLE_PALETTE = 1,
    TWINKLE_RANDOM = 2
};

// Fairy Lights modes
enum FairyMode {
    FAIRY_WARM_WHITE = 0,
    FAIRY_COLD_WHITE = 1,
    FAIRY_MULTICOLOR = 2,
    FAIRY_PALETTE = 3
};

// Christmas patterns
enum ChristmasPattern {
    XMAS_ALTERNATING = 0,
    XMAS_CHASE = 1,
    XMAS_SPARKLE = 2
};

// Police styles
enum PoliceStyle {
    POLICE_SINGLE = 0,
    POLICE_SOLID = 1,
    POLICE_ALTERNATING = 2
};

// Strobe modes
enum StrobeMode {
    STROBE_NORMAL = 0,
    STROBE_MEGA = 1,
    STROBE_RAINBOW = 2
};

// Palette types
enum PaletteType {
    PALETTE_RAINBOW = 0,
    PALETTE_PARTY = 1,
    PALETTE_OCEAN = 2,
    PALETTE_FOREST = 3,
    PALETTE_LAVA = 4,
    PALETTE_HEAT = 5,
    PALETTE_CLOUD = 6,
    PALETTE_SNOW = 7,
    PALETTE_AURORA = 8,
    PALETTE_SUNSET = 9,
    PALETTE_RETRO = 10,
    PALETTE_CHRISTMAS = 11,
    PALETTE_HALLOWEEN = 12,
    PALETTE_CYBER = 13
};

// ============== PARAMETER STRUCTURES ==============

// --- CATEGORY 1: STATIC EFFECTS ---

struct SolidParams {
    CRGB color;
    uint8_t brightness;
};

struct GradientParams {
    CRGB colorStart;
    CRGB colorMiddle;
    CRGB colorEnd;
    GradientStyle style;
    bool threePoint;            // Whether to use colorMiddle
};

struct SpotsParams {
    CRGB color;
    uint8_t spread;             // Spacing between spots (1-30)
    uint8_t width;              // Spot width in LEDs (1-10)
    bool fade;                  // Fading edges
};

struct PatternParams {
    CRGB colorFg;
    CRGB colorBg;
    uint8_t fgSize;             // Foreground size (1-20)
    uint8_t bgSize;             // Background size (1-20)
};

// --- CATEGORY 2: WAVE EFFECTS ---

struct RainbowWaveParams {
    uint8_t speed;              // 0-255
    uint8_t size;               // Rainbow cycle width (1-50)
    Direction direction;
    uint8_t saturation;         // 0-255
};

struct ColorWaveParams {
    CRGB colors[8];
    uint8_t numColors;          // Number of colors used (2-8)
    uint8_t speed;
    Direction direction;
};

struct OscillateParams {
    CRGB colorPrimary;
    CRGB colorSecondary;
    uint8_t speed;
    uint8_t pointSize;  // Size of oscillating point (1-20)
};

struct WavyParams {
    PaletteType palette;
    uint8_t speed;
    uint8_t amplitude;          // Sine wave amplitude (1-255)
    uint8_t frequency;          // Number of waves (1-10)
};

// --- CATEGORY 3: CHASE/RUNNING EFFECTS ---

struct TheaterChaseParams {
    CRGB color;
    uint8_t speed;
    uint8_t gapSize;            // Gap between lights (1-10)
    bool rainbowMode;           // Rainbow instead of single color
};

struct ScannerParams {
    CRGB colors[8];             // Array of colors for multi-color dots
    uint8_t speed;
    uint8_t numDots;            // Number of dots (1-8)
    uint8_t trailLength;        // Trail length (1-50)
    bool dualMode;              // Scanning from both sides
    bool overlay;               // Overlay on background
};

struct CometParams {
    CRGB color;
    CRGB sparkleColor;          // Sparkle color in trail
    uint8_t speed;
    uint8_t trailLength;        // Trail length (1-50)
    bool sparkleEnabled;        // Enable sparkling
    Direction direction;
};

struct RunningLightsParams {
    CRGB colors[4];             // Array of colors for multi-color waves
    uint8_t numColors;          // Number of colors to use (1-4)
    uint8_t speed;
    uint8_t waveWidth;          // Wave width (1-50)
    bool dualMode;              // Waves from both directions
    WaveShape shape;
};

struct AndroidParams {
    CRGB colorPrimary;
    CRGB colorSecondary;
    uint8_t speed;
    uint8_t sectionWidth;       // Strip percentage (1-50%)
};

// --- CATEGORY 4: TWINKLE/SPARKLE EFFECTS ---

struct TwinkleParams {
    CRGB twinkleColor;          // Color for single-color mode
    PaletteType palette;
    uint8_t speed;
    uint8_t intensity;          // Number of active LEDs (0-255)
    uint8_t fadeSpeed;          // Fade speed (0-255)
    TwinkleMode colorMode;
};

struct TwinkleFoxParams {
    PaletteType palette;
    uint8_t speed;
    uint8_t twinkleRate;        // Appearance frequency
    uint8_t fadeOut;            // Fade speed
};

struct SparkleParams {
    CRGB colorSpark;
    CRGB colorBg;
    uint8_t speed;
    uint8_t intensity;          // Number of sparkles (0-255)
    bool overlay;               // Overlay on base effect
    bool darkMode;              // Dark sparkles instead of bright
};

struct GlitterParams {
    uint8_t intensity;          // Number of glitters (0-255)
    bool rainbowBg;             // Rainbow background
    CRGB bgColor;               // Or solid background color
    bool overlay;
};

struct StarryNightParams {
    uint8_t speed;
    uint8_t density;            // Number of stars (0-255)
    CRGB colorStars;
    bool shootingStars;         // Shooting stars
};

// --- CATEGORY 5: FIRE/ORGANIC EFFECTS ---

struct FireParams {
    uint8_t cooling;            // How quickly it cools (20-100)
    uint8_t sparking;           // Spark frequency (50-200)
    bool boost;                 // Flame boost
    PaletteType palette;
};

struct CandleParams {
    uint8_t speed;
    uint8_t intensity;          // Flicker intensity (0-255)
    bool multiMode;             // Each LED as separate candle
    CRGB color;
    uint8_t colorShift;         // Color variation (0-100)
};

struct FireFlickerParams {
    uint8_t speed;
    uint8_t intensity;
    CRGB color;
};

struct LavaParams {
    uint8_t speed;              // Slow (20-80)
    uint8_t blobSize;           // Blob size (5-40)
    uint8_t smoothness;         // Transition smoothness (100-255)
};

struct AuroraParams {
    uint8_t speed;
    uint8_t intensity;
    PaletteType palette;
};

struct PacificaParams {
    uint8_t speed;
    PaletteType palette;
};

struct LakeParams {
    uint8_t speed;
    PaletteType palette;
};

// --- CATEGORY 6: HOLIDAY EFFECTS ---

struct FairyParams {
    uint8_t speed;
    uint8_t numFlashers;        // Number of flashing LEDs (1-NUM_LEDS)
    FairyMode colorMode;
    PaletteType palette;
};

struct ChristmasChaseParams {
    uint8_t speed;
    CRGB color1;
    CRGB color2;
    ChristmasPattern pattern;
};

struct HalloweenEyesParams {
    uint16_t duration;          // Glow duration (ms)
    uint16_t fadeTime;          // Fade time (ms)
    CRGB color;
    bool overlay;
};

struct FireworksParams {
    uint8_t chance;             // Explosion frequency (0-255)
    uint8_t fragments;          // Number of fragments (4-16)
    uint8_t gravity;            // Gravity strength (0-255)
    bool overlay;
};

struct SnowSparkleParams {
    uint8_t speed;
    uint8_t density;            // Flake density (0-255)
    CRGB color;
    Direction direction;
};

// --- CATEGORY 7: SPECIAL EFFECTS ---

struct BouncingBallsParams {
    uint8_t gravity;            // Gravity strength (100-255)
    uint8_t numBalls;           // Number of balls (1-8)
    bool overlay;
    uint8_t trail;              // Trail length (0-20)
    PaletteType palette;
};

struct PopcornParams {
    uint8_t speed;
    uint8_t intensity;          // Number of kernels (0-255)
    PaletteType palette;
};

struct DripParams {
    uint8_t gravity;            // Gravity strength (100-255)
    uint8_t numDrips;           // Number of drips (1-8)
    bool overlay;
    CRGB color;
};

struct PlasmaParams {
    uint8_t phase;              // Initial phase (0-255)
    uint8_t intensity;          // Effect intensity (0-255)
    uint8_t speed;
};

struct LightningParams {
    uint8_t frequency;          // Flash frequency (0-255)
    uint8_t intensity;          // Flash brightness (0-255)
    CRGB color;
    bool overlay;
};

struct MatrixParams {
    uint8_t speed;
    uint8_t spawningRate;       // New drop frequency (0-255)
    uint8_t trailLength;        // Trail length (3-30)
    CRGB color;                 // Drop color
};

struct HeartbeatParams {
    uint8_t bpm;                // Simulated BPM (40-180)
    CRGB color;
};

// --- CATEGORY 8: BREATHING/FADE EFFECTS ---

struct BreatheParams {
    uint8_t speed;              // Breathing cycle (20-200)
    CRGB colorPrimary;
    CRGB colorSecondary;
    bool twoColor;              // Whether to transition between two colors
};

struct DissolveParams {
    uint8_t repeatSpeed;        // Cycle time (50-200)
    uint8_t dissolveSpeed;      // Dissolve speed (50-200)
    bool randomColors;          // Random colors
    CRGB color;                 // Or solid color
};

struct FadeParams {
    uint8_t speed;
    CRGB colors[8];
    uint8_t numColors;          // Number of colors (2-8)
    bool loop;                  // Looping
};

// --- CATEGORY 9: ALERT EFFECTS ---

struct PoliceLightsParams {
    uint8_t speed;
    CRGB color1;
    CRGB color2;
    PoliceStyle style;
};

struct StrobeParams {
    uint8_t frequency;          // Flash frequency (50-255)
    CRGB color;
    StrobeMode mode;
};

// ============== EXTERN DECLARATIONS ==============
// Access to parameters from other files

extern SolidParams solidParams;
extern GradientParams gradientParams;
extern SpotsParams spotsParams;
extern PatternParams patternParams;

extern RainbowWaveParams rainbowWaveParams;
extern ColorWaveParams colorWaveParams;
extern OscillateParams oscillateParams;
extern WavyParams wavyParams;

extern TheaterChaseParams theaterChaseParams;
extern ScannerParams scannerParams;
extern CometParams cometParams;
extern RunningLightsParams runningLightsParams;
extern AndroidParams androidParams;

extern TwinkleParams twinkleParams;
extern TwinkleFoxParams twinkleFoxParams;
extern SparkleParams sparkleParams;
extern GlitterParams glitterParams;
extern StarryNightParams starryNightParams;

extern FireParams fireParams;
extern CandleParams candleParams;
extern FireFlickerParams fireFlickerParams;
extern LavaParams lavaParams;
extern AuroraParams auroraParams;
extern PacificaParams pacificaParams;
extern LakeParams lakeParams;

extern FairyParams fairyParams;
extern ChristmasChaseParams christmasChaseParams;
extern HalloweenEyesParams halloweenEyesParams;
extern FireworksParams fireworksParams;
extern SnowSparkleParams snowSparkleParams;

extern BouncingBallsParams bouncingBallsParams;
extern PopcornParams popcornParams;
extern DripParams dripParams;
extern PlasmaParams plasmaParams;
extern LightningParams lightningParams;
extern MatrixParams matrixParams;
extern HeartbeatParams heartbeatParams;

extern BreatheParams breatheParams;
extern DissolveParams dissolveParams;
extern FadeParams fadeParams;

extern PoliceLightsParams policeLightsParams;
extern StrobeParams strobeParams;

// Global variables accessible to effects
extern CRGB leds[];
extern uint32_t frameCounter;
extern uint32_t lastFrameTime;

#endif // EFFECT_PARAMS_H
