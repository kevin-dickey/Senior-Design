#define USE_EMULATOR 0

#include <iostream>

#define LED_PIN         13
#define NUM_LEDS_X      16
#define NUM_LEDS_Y      16
#define NUM_LEDS        100
#define MAX_BRIGHTNESS  255 // maximum for FastLED is 255, but I would probably not go higher than 64 (ESPECIALLY if no power supply)


# if USE_EMULATOR

#include "../ImGUI_Emulator/Window.h"

# else

#include <FastLED.h>

#define COLOR_ORDER     RBG
#define CHIPSET         WS2812

# endif


/* Function Prototypes */
void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y, int rippleCounter); 
uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter); // depricated function
uint16_t XY(uint8_t x, uint8_t y);
uint16_t XYsafe(uint8_t x, uint8_t y);


/* Variables for XY() and XYsafe() */
// Params for width and height
const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;
// Param for different pixel layouts
const bool kMatrixSerpentineLayout = true;
const bool kMatrixVertical = false;

// Array of the LEDs. Should be accessed using the XY functions (translation to 2D array, which is not done directly b/c
//                                                               of different possible layouts of the LEDs (serpentine n such))
CRGB leds[NUM_LEDS];

int hue;


# if USE_EMULATOR
void loop_callback() {

    // modified call to meet new method signature
    static int rippleCountah = 0;
    rippleEffect(255, 0, 255, NUM_LEDS_X / 2, NUM_LEDS_Y / 2, rippleCountah); // purple :D
    std ::cout << "Ripple effect frame 1/13" << std::endl;
}

int main() {
    emulator(loop_callback);
}
# else

/**
 * MARK: Setup
*/
void setup() {
  Serial.begin(9600); // for setting up stuff to print to serial monitor
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050); // setup the LEDs & LED pin for the esp32
  FastLED.setBrightness(MAX_BRIGHTNESS); // set the max brightness for the LEDs

  hue = 30;
}

/**
 * MARK: Looping
*/
void loop() {
    fill_rainbow(leds, NUM_LEDS, hue, 7); // fill the LEDs with a rainbow effect
    hue += 1; // increment the hue for the next frame
    hue %= 256; // keep the hue within the range of 0-255
    FastLED.show(); // show the LEDs
    delay(1000 / 60); // delay for 60fps
}
# endif


/**
 * MARK:  Ripple effect
 */
/**
 * 
 * Provides a singular frame for the ripple effect.
 * Should be repeatedly called in a loop for a smooth ripple.
 * Currently has overlapping frames near the end of a ripple's lifespan (believe frames 11-13)
 *
 *    r = 0-255 value, specifying the amount of red
 *    g = 0-255 value, specifying the amount of green
 *    b = 0-255 value, specifying the amount of blue
 *
 * Provide values 0-255 for specifying the color in terms of r, g, and b.
 * Do NOT adjust them for brightness, JUST COLOR. (nothing bad will happen just won't work as expected)
 * If you want to adjust the brightness of the LEDs, adjust MAX_BRIGHTNESS accordingly.
 */ 
void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y, int rippleCounter) {
  uint8_t maxDistance = max(NUM_LEDS_X, NUM_LEDS_Y);

  // Iterate through the LED matrix
  for (uint8_t x = 0; x < NUM_LEDS_X; x++) {
    for (uint8_t y = 0; y < NUM_LEDS_Y; y++) {
      // Calculate distance and ripple distance
      uint8_t distance = calculateDistance(center_x, center_y, x, y);
      uint8_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance + 1);
      uint8_t brightness;

      // Determine brightness based on distance from center and rippleCounter
      if (rippleDistance <= 1) {
        brightness = MAX_BRIGHTNESS;
      } else {
        brightness = 0;  // Dim brightness value outside the ripple's ring
      }

      // Create a CRGB object with the calculated color and brightness
      CRGB newColor = CRGB(r * brightness / MAX_BRIGHTNESS, g * brightness / MAX_BRIGHTNESS, b * brightness / MAX_BRIGHTNESS);

      // Add the newColor to the existing LED color using blend function
      leds[XY(x, y)] += newColor;
    }
  }
}


/**
 * MARK: Distance calculation
*/
/**
 * Calculates the distance between two (x, y) points provided.
*/
uint8_t calculateDistance(uint8_t center_x, uint8_t center_y, uint8_t x, uint8_t y) {
    // Calculate Euclidean distance from center point to point (x, y)
    int dx = x - center_x;
    int dy = y - center_y;
    return static_cast<uint8_t>(sqrt(dx * dx + dy * dy));
}



/**
 * Scales the brightness of the LEDs based on the distance from the center of the actual ripple in the frame
 * For example, if the width of the ripple is 3 pixels wide, the center would be brightest and the 2 outside
 * pixels would be dimmed slightly.
 *
 * (Not sure if it's actually working as intended to be hoenst :D)
 * (Effectively Depricated)
*/
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter) {
  // uint8_t delta = abs(rippleCounter - distance);
  // uint8_t maxDistance = NUM_LEDS / 2;
  // uint8_t brightness = map(delta, 0, maxDistance, 0, MAX_BRIGHTNESS);
  // return (brightness <= 0) ? 0 : (brightness > 2) ? 2 : brightness; // ensures 0 <= brightness <= 16
  // return brightness > MAX_BRIGHTNESS ? 0 : brightness;
  return 0;
}


/**
 * Calculates the (x, y) position of a grid of LEDs.
 * This is required over just creating a 2D array as it depends whether the LEDs
 * are setup in a serpentine manner, and if they're setup ordered in a vertical manner or not.
 * 
 * If something doesn't look right, try changing the value of kMatrixVertical above.
 * If that doesn't work, try changing kMatrixSerpentineLayout (not applicable for testbench, we know the value it needs to be).
*/
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
  };
  
  return i;
}


/**
 * Makes sure the specified point is in bounds before calculating its (x, y) position.
*/
uint16_t XYsafe(uint8_t x, uint8_t y) {
    if (x >= kMatrixWidth) return -1;
    if (y >= kMatrixHeight) return -1;
    return XY(x, y);
}


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
