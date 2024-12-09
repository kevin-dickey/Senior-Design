#ifndef PIXELCONTROLLER_UTILS_H
#define PIXELCONTROLLER_UTILS_H

#include <stdint.h>
#include <FastLED.h>
#include "FileManager.h"
#include <vector>

uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter);  // depricated function
uint16_t XY(uint8_t x, uint8_t y);
uint16_t XYsafe(uint8_t x, uint8_t y);
void rearrangeForSerpentine(CRGB* originalArray, CRGB* rearrangedArray, int width, int height);
void parseBitmapData(const char *hexData);
uint8_t bufferToCRGBArray(unsigned char *buffer, int imgWidth, int imgHeight, int imgChannels, CRGB *leds, int matrixWidth, int matrixHeight, int startX, int startY, bool wrap = false);
void loadImagesFromSD(std::vector<std::string> images, FileManager *fm, int leds_x, int leds_y);
void fillRemainingPixels(CRGB *leds, int matrixWidth, int matrixHeight, CRGB backgroundColor);
void loadHexBitmap(CRGB *leds, const char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth);
CRGB hexToCRGB(const char *hex);
void resetTriggerMarkers(int exception);


/* Variables for XY() and XYsafe() */
// Params for width and height
extern uint8_t kMatrixWidth; 
extern uint8_t kMatrixHeight;
// Param for different pixel layouts
const bool kMatrixSerpentineLayout = false; // <-- this should be passed thru json file :)
const bool kMatrixVertical = false;
/***********************************************************************************************************/

// #define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
// CRGB* const leds( leds_plus_safety_pixel + 1);

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