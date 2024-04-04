
#include <FastLED.h>
#include <iostream>

#define LED_PIN     22
#define NUM_LEDS_X  16
#define NUM_LEDS_Y  16
#define NUM_LEDS    NUM_LEDS_X * NUM_LEDS_Y
#define MAX_BRIGHTNESS  16 // maximum for FastLED is 255, but I would probably not go higher than 64 (especially if no power supply)
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B

void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y); // provide values 0-255 for specifying the color in terms of r, g, and b.
                                        // do NOT adjust them for brightness, JUST COLOR. (nothing bad will happen just won't work as expected)
uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter);
uint16_t XY(uint8_t x, uint8_t y);
uint16_t XYsafe(uint8_t x, uint8_t y);

/* Variables for XY() and XYsafe() */
// Params for width and height
const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;
// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;
const bool    kMatrixVertical = false;

// Array of the LEDs. Should be accessed using the XY functions (translation to 2D array, which is not done directly b/c
//                                                               of different possible layouts of the LEDs (serpentine n such))
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600); // for setting up stuff to print to serial monitor
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050); // setup the LEDs & LED pin for the esp32
  FastLED.setBrightness(MAX_BRIGHTNESS); // set the max brightness for the LEDs
}

void loop() {
  rippleEffect(255, 0, 255, NUM_LEDS_X / 2, NUM_LEDS_Y / 2); // purple :D
  FastLED.show();
  delay(75);  // adjust delay for speed of the ripple effect  
}

void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y) {
    static int rippleCounter = 0;
    // uint8_t center_x = NUM_LEDS_X / 2;
    // uint8_t center_y = NUM_LEDS_Y / 2;
    
    uint8_t maxDistance = max(NUM_LEDS_X / 2, NUM_LEDS_Y / 2);

    for (uint8_t x = 0; x < NUM_LEDS_X; x++) {
        for (uint8_t y = 0; y < NUM_LEDS_Y; y++) {
            uint8_t distance = calculateDistance(center_x, center_y, x, y);
            uint8_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance + 1);
            uint8_t brightness;

            // Determine brightness based on distance from center and rippleCounter
            if (rippleDistance <= 1) {
                // If the pixel is within the ring
                brightness = MAX_BRIGHTNESS - rippleDistance * 85; // Gradually decrease brightness towards the edge of the ring
            } else {
                // If the pixel is outside the ring
                brightness = 0; // Dim brightness value
            }
          
            leds[XY(x, y)] = CRGB(r * brightness / MAX_BRIGHTNESS, g * brightness / MAX_BRIGHTNESS, b * brightness / MAX_BRIGHTNESS);  // Adjust color as needed
        }
    }

    // debugging
    Serial.printf("Now showing frame %d/13 of ripple %d.\n", (rippleCounter % 13 + 1), (rippleCounter / 13 + 1));
    rippleCounter++;
}

// void rippleEffect() {
//   static int rippleCounter = 0;
//   uint8_t center_x = NUM_LEDS_X / 2;
//   uint8_t center_y = NUM_LEDS_Y / 2;

//   for (int x = 0; x < NUM_LEDS; x++) {
//     for (int y = 0; y < NUM_LEDS; y++) {
//       uint8_t distance = calculateDistance(center_x, center_y, x, y);
//       uint8_t brightness = scaleBrightness(distance, rippleCounter);
//       leds[XYsafe(x, y)] = CRGB(0, 0, brightness); // just different brightnesses of blue
//     }
//   }

//   rippleCounter++;
// }

uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  float dx = abs(x2 - x1);
  float dy = abs(y2 - y1);
  return sqrt(dx * dx + dy * dy);
}

uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter) {
  uint8_t delta = abs(rippleCounter - distance);
  uint8_t maxDistance = NUM_LEDS / 2;
  uint8_t brightness = map(delta, 0, maxDistance, 0, MAX_BRIGHTNESS);
  // return (brightness <= 0) ? 0 : (brightness > 2) ? 2 : brightness; // ensures 0 <= brightness <= 16
  return brightness > MAX_BRIGHTNESS ? 0 : brightness;
}

uint16_t XY(uint8_t x, uint8_t y)
{
  int i;
  
  if( kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * kMatrixWidth) + x;
    } else {
      i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
    }
  }

  if( kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if( y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (kMatrixWidth - 1) - x;
        i = (y * kMatrixWidth) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * kMatrixWidth) + x;
      }
    } else { // vertical positioning
      if ( x & 0x01) {
        i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
      } else {
        i = kMatrixHeight * (kMatrixWidth - x) - (y+1);
      }
    }
  }
  
  return i;
}

uint16_t XYsafe(uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}


/* *********************************************************************** */
// TODO:
/* Try these changes later */

// uint8_t calculateDistance(uint8_t center_x, uint8_t center_y, uint8_t x, uint8_t y) {
//   // Calculate the distances from the center to the edges of the matrix
//   uint8_t center_to_left_edge = center_x;
//   uint8_t center_to_right_edge = NUM_LEDS_X - 1 - center_x;
//   uint8_t center_to_top_edge = center_y;
//   uint8_t center_to_bottom_edge = NUM_LEDS_Y - 1 - center_y;

//   // Calculate the distances from the point (x, y) to the edges of the matrix
//   uint8_t point_to_left_edge = x;
//   uint8_t point_to_right_edge = NUM_LEDS_X - 1 - x;
//   uint8_t point_to_top_edge = y;
//   uint8_t point_to_bottom_edge = NUM_LEDS_Y - 1 - y;

//   // Calculate the minimum distances from the point to the center
//   uint8_t min_x_distance = min(abs(int(center_x) - int(x)), min(center_to_left_edge + point_to_right_edge, center_to_right_edge + point_to_left_edge));
//   uint8_t min_y_distance = min(abs(int(center_y) - int(y)), min(center_to_top_edge + point_to_bottom_edge, center_to_bottom_edge + point_to_top_edge));

//   return max(min_x_distance, min_y_distance);
// }

// void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y) {
//   static int rippleCounter = 0;
//   uint8_t maxDistance = max(NUM_LEDS_X, NUM_LEDS_Y);

//   for (uint8_t x = 0; x < NUM_LEDS_X; x++) {
//     for (uint8_t y = 0; y < NUM_LEDS_Y; y++) {
//       uint8_t distance = calculateDistance(center_x, center_y, x, y);
//       uint8_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance + 1);
//       uint8_t brightness;

//       // Determine brightness based on distance from center and rippleCounter
//       if (rippleDistance <= 1) {
//         // If the pixel is within the ring
//         brightness = MAX_BRIGHTNESS - rippleDistance * 85;  // Gradually decrease brightness towards the edge of the ring
//       } else {
//         // If the pixel is outside the ring
//         brightness = 0;  // Dim brightness value
//       }

//       leds[XY(x, y)] = CRGB(r * brightness / MAX_BRIGHTNESS, g * brightness / MAX_BRIGHTNESS, b * brightness / MAX_BRIGHTNESS);  // Adjust color as needed
//     }
//   }

//   // debugging
//   Serial.printf("Now showing frame %d/13 of ripple %d.\n", (rippleCounter % 13 + 1), (rippleCounter / 13 + 1));
//   rippleCounter++;
// }


// TODO:
/* Also try these changes later */

// uint8_t calculateDistance(uint8_t center_x, uint8_t center_y, uint8_t x, uint8_t y) {
//     int dx = abs(x - center_x);
//     int dy = abs(y - center_y);
//     return max(dx, dy); // Distance is the maximum of horizontal and vertical distances
// }

// void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y) {
//     static int rippleCounter = 0;

//     for (uint8_t x = 0; x < NUM_LEDS_X; x++) {
//         for (uint8_t y = 0; y < NUM_LEDS_Y; y++) {
//             uint8_t distance = calculateDistance(center_x, center_y, x, y);
//             uint8_t rippleDistance = (rippleCounter + distance) % 13;
//             uint8_t brightness;

//             // Determine brightness based on distance from center and rippleCounter
//             if (rippleDistance <= 1) {
//                 // If the pixel is within the ring
//                 brightness = MAX_BRIGHTNESS - rippleDistance * 85; // Gradually decrease brightness towards the edge of the ring
//             } else {
//                 // If the pixel is outside the ring
//                 brightness = 0; // Dim brightness value
//             }
          
//             leds[XY(x, y)] = CRGB(r * brightness / MAX_BRIGHTNESS, g * brightness / MAX_BRIGHTNESS, b * brightness / MAX_BRIGHTNESS);  // Adjust color as needed
//         }
//     }

//     // debugging
//     Serial.printf("Now showing frame %d/13 of ripple %d.\n", (rippleCounter % 13 + 1), (rippleCounter / 13 + 1));
//     rippleCounter++;
// }





/* Deprecated code */

// #define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
// CRGB* const leds( leds_plus_safety_pixel + 1);