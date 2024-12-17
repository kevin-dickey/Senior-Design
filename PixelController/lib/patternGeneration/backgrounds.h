#ifndef PIXELCONTROLLER_BACKGROUNDS_H
#define PIXELCONTROLLER_BACKGROUNDS_H

// TODO: Make includes emulator-friendly
#include <vector>
#include <FastLED.h>

#include "utils.h"

void fillFrameWithColorPattern(
    CRGB* frame,          // 2D array of CRGB pixels 
    int height,            // Number of rows
    int width,             // Number of columns
    const std::vector<CRGB>& colors,  // Color pattern
    int stride,            // Width of each color block
    ShiftDirection shiftDirection 
);

// Version 2
// void fillFrameWithColorPattern(
//     CRGB** frame,          // 2D array of CRGB pixels 
//     int height,            // Number of rows
//     int width,             // Number of columns
//     const std::vector<CRGB>& colors,  // Color pattern
//     int stride,            // Width of each color block
//     ShiftDirection shiftDirection,
//     int skipFirstWidth = 0  // Offset for the first row's start
// );

#endif