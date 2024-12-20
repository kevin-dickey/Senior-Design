#ifndef PIXELCONTROLLER_RIPPLEEFFECT_H
#define PIXELCONTROLLER_RIPPLEEFFECT_H

#include <stdint.h>

#if USE_EMULATOR
#include "patternGeneration/utils.h"
#else
#include <FastLED.h>
#include <utils.h>
#endif

void rippleEffect(CRGB *frame_data,
                   int num_leds_x, int num_leds_y, 
                   int r, int g, int b, 
                   uint8_t center_x, uint8_t center_y, 
                   int rippleCounter, int prevLeds[], 
                   int width);

#endif // PIXELCONTROLLER_RIPPLEEFFECT_H