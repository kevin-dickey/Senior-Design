#ifndef PIXELCONTROLLER_RIPPLEEFFECT_H
#define PIXELCONTROLLER_RIPPLEEFFECT_H

#include <stdint.h>

#include <FastLED.h>
#include "utils.h"

CRGB* rippleEffect(CRGB *leds, int *num_leds, int r, int g, int b, uint8_t center_x, uint8_t center_y, int rippleCounter, int prevLeds[], int width);

#endif // PIXELCONTROLLER_RIPPLEEFFECT_H