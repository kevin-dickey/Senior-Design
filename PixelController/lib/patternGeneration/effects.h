#ifndef PIXELCONTROLLER_EFFECTS_H
#define PIXELCONTROLLER_EFFECTS_H

#include <FastLED.h>
#include <stdint.h>
#include <vector>

#include "ControllerRunner.h"
#include "utils.h"

enum ShiftDirection {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

void DrawOneFrameReducedBright(CRGB* frame, uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8);  // ^ @ slightly lower brightness for readability of other things on leds (e.g. pumpkin more easily visible)
void drawRainbow(CRGB* frame, unsigned long current_millis);
void fadeToBlack(int duration);
void fadeToBrightness(int duration, int targetBrightness);
void loadHexBitmap(CRGB *leds, const char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth);
void shiftLeds(CRGB leds[], int leds_x, int leds_y, ShiftDirection direction);

#endif
