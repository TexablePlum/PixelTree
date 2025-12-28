/*
 * Palettes.h - Color palettes definitions
 */

#ifndef PALETTES_H
#define PALETTES_H

#include <FastLED.h>
#include "EffectParams.h"

// ============== CUSTOM PALETTES ==============

// Aurora
DEFINE_GRADIENT_PALETTE(aurora_gp) {
    0,   0,   0,   0,      // Black
    32,  0,  64,   0,      // Dark green
    64,  0, 255,  64,      // Light green
    96,  0, 128, 255,      // Cyan-blue
    128, 64,   0, 255,     // Violet
    160, 128,  0, 128,     // Magenta
    192, 0, 255, 128,      // Green-cyan
    224, 0,  64, 128,      // Dark cyan
    255, 0,   0,   0       // Black
};

// Sunset
DEFINE_GRADIENT_PALETTE(sunset_gp) {
    0,   255, 128,   0,    // Orange
    64,  255,  64,   0,    // Dark orange
    128, 255,   0,  64,    // Red-orange
    192, 128,   0, 128,    // Violet
    255,  64,   0,  64     // Dark violet
};

// Retro
DEFINE_GRADIENT_PALETTE(retro_gp) {
    0,   255,   0, 128,    // Hot pink
    64,  128,   0, 255,    // Violet
    128,   0,   0, 255,    // Blue
    192,   0, 128, 255,    // Cyan
    255, 255,   0, 128     // Hot pink (loop)
};

// Christmas
DEFINE_GRADIENT_PALETTE(christmas_gp) {
    0,   255,   0,   0,    // Red
    64,  255,   0,   0,    // Red
    96,  255, 255, 255,    // White
    128,   0, 255,   0,    // Green
    192,   0, 255,   0,    // Green
    224, 255, 255, 255,    // White
    255, 255,   0,   0     // Red
};

// Halloween
DEFINE_GRADIENT_PALETTE(halloween_gp) {
    0,   255,  64,   0,    // Orange
    64,  128,   0, 128,    // Violet
    128,   0,   0,   0,    // Black
    192, 128,   0, 128,    // Violet
    255, 255,  64,   0     // Orange
};

// Cyber - Neon/Cyberpunk
DEFINE_GRADIENT_PALETTE(cyber_gp) {
    0,     0, 255, 255,    // Cyan
    64,  255,   0, 255,    // Magenta
    128,   0, 255, 128,    // Neon green
    192, 255, 255,   0,    // Yellow
    255,   0, 255, 255     // Cyan
};

// Snow - Snow (shades of white and blue)
DEFINE_GRADIENT_PALETTE(snow_gp) {
    0,   255, 255, 255,    // White
    64,  200, 200, 255,    // Light blue
    128, 255, 255, 255,    // White
    192, 180, 180, 255,    // Blue
    255, 255, 255, 255     // White
};

// ============== FUNCTION FOR GETTING PALETTE ==============

CRGBPalette16 getPalette(PaletteType type) {
    switch (type) {
        case PALETTE_RAINBOW:
            return RainbowColors_p;
            
        case PALETTE_PARTY:
            return PartyColors_p;
            
        case PALETTE_OCEAN:
            return OceanColors_p;
            
        case PALETTE_FOREST:
            return ForestColors_p;
            
        case PALETTE_LAVA:
            return LavaColors_p;
            
        case PALETTE_HEAT:
            return HeatColors_p;
            
        case PALETTE_CLOUD:
            return CloudColors_p;
            
        case PALETTE_SNOW:
            return snow_gp;
            
        case PALETTE_AURORA:
            return aurora_gp;
            
        case PALETTE_SUNSET:
            return sunset_gp;
            
        case PALETTE_RETRO:
            return retro_gp;
            
        case PALETTE_CHRISTMAS:
            return christmas_gp;
            
        case PALETTE_HALLOWEEN:
            return halloween_gp;
            
        case PALETTE_CYBER:
            return cyber_gp;
            
        default:
            return RainbowColors_p;
    }
}

// Palette name (for debugging)
const char* getPaletteName(PaletteType type) {
    static const char* names[] = {
        "Rainbow", "Party", "Ocean", "Forest",
        "Lava", "Heat", "Cloud", "Snow",
        "Aurora", "Sunset", "Retro", "Christmas",
        "Halloween", "Cyber"
    };
    if (type <= PALETTE_CYBER) {
        return names[type];
    }
    return "Unknown";
}

#endif // PALETTES_H
