#ifndef PIXELCONTROLLER_UTILS_H
#define PIXELCONTROLLER_UTILS_H

#include <stdint.h>
#include <FastLED.h>
#include <vector>

uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter);  // depricated function
uint16_t XY(uint8_t x, uint8_t y);
uint16_t XYsafe(uint8_t x, uint8_t y);
void rearrangeForSerpentine(CRGB* originalArray, CRGB* rearrangedArray, int width, int height);
void rearrangeForStrips(CRGB* originalArray, CRGB** rearrangedArrays, int width, int height);


/* Variables for XY() and XYsafe() */
// Params for width and height
extern uint8_t kMatrixWidth; 
extern uint8_t kMatrixHeight;
// Param for different pixel layouts
const bool kMatrixSerpentineLayout = false; // <-- this should be passed thru json file :)
const bool kMatrixVertical = false;
/***********************************************************************************************************/

// #define num_leds (kMatrixWidth * kMatrixHeight)
// CRGB leds_plus_safety_pixel[ num_leds + 1];
// CRGB* const frame_data( leds_plus_safety_pixel + 1);

// Set 'kMatrixSerpentineLayout' to false if your pixels are
// laid out all running the same way, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//     .----<----<----<----'
//     |
//     5 >  6 >  7 >  8 >  9
//                         |
//     .----<----<----<----'
//     |
//    10 > 11 > 12 > 13 > 14
//                         |
//     .----<----<----<----'
//     |
//    15 > 16 > 17 > 18 > 19
//
// Set 'kMatrixSerpentineLayout' to true if your pixels are
// laid out back-and-forth, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//                         |
//     9 <  8 <  7 <  6 <  5
//     |
//     |
//    10 > 11 > 12 > 13 > 14
//                        |
//                        |
//    19 < 18 < 17 < 16 < 15

#endif //PIXELCONTROLLER_UTILS_H