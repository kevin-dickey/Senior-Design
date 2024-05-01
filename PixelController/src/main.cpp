#define USE_EMULATOR 0

#include <iostream>

#define LED_PIN 13
#define NUM_LEDS_X 16
#define NUM_LEDS_Y 16
#define NUM_LEDS NUM_LEDS_X *NUM_LEDS_Y
#define MAX_BRIGHTNESS 6  // maximum for FastLED is 255, but I would probably not go higher than 64 (ESPECIALLY if no power supply)

/* Declarations for the buttons, number corresponds to pin on the ESP32 */
#define INPUT_BTN_NE 34
#define INPUT_BTN_NW 35
#define INPUT_BTN_SW 32
#define INPUT_BTN_SE 33

#if USE_EMULATOR

#include "../ImGUI_Emulator/Window.h"

#else

#include <FastLED.h>

#define COLOR_ORDER GRB
#define CHIPSET WS2812B

#endif

void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y, int rippleCounter, int prevLeds[], int width);
uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter);  // depricated function
uint16_t XY(uint8_t x, uint8_t y);
uint16_t XYsafe(uint8_t x, uint8_t y);
void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8);  // draws rainbow frame

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
int prevLeds1[NUM_LEDS] = {0};
int prevLeds2[NUM_LEDS] = {0};
int prevLeds3[NUM_LEDS] = {0};
int prevLeds4[NUM_LEDS] = {0};

#if USE_EMULATOR
void loop_callback() {
  // modified call to meet new method signature
  static int rippleCountah = 0;
  rippleEffect(255, 0, 255, NUM_LEDS_X / 2, NUM_LEDS_Y / 2, rippleCountah);  // purple :D
  std ::cout << "Ripple effect frame 1/13" << std::endl;
}

int main() {
  emulator(loop_callback);
}
#else

/**
 * MARK: Setup
 */
void setup() {
  Serial.begin(115200);                                                                          // for setting up stuff to print to serial monitor
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);  // setup the LEDs & LED pin for the esp32
  FastLED.setBrightness(MAX_BRIGHTNESS);                                                         // set the max brightness for the LEDs
  pinMode(INPUT_BTN_NE, INPUT);
  pinMode(INPUT_BTN_NW, INPUT);
  pinMode(INPUT_BTN_SE, INPUT);
  pinMode(INPUT_BTN_SW, INPUT);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

bool button1_status;
bool button2_status;
bool button3_status;
bool button4_status;

/**
 * MARK: Looping
 */
void loop() {
  static int rippleCounter1 = 0;  // could also be considered the particular ripple effect's "id"
  static int rippleCounter2 = 0;
  static int rippleCounter3 = 0;
  static int rippleCounter4 = 0;
  button1_status = digitalRead(INPUT_BTN_NE);
  button2_status = digitalRead(INPUT_BTN_NW);
  button3_status = digitalRead(INPUT_BTN_SE);
  button4_status = digitalRead(INPUT_BTN_SW);
  int width = 2;

  if (button1_status) {
    if (rippleCounter1 == NUM_LEDS_X + width + 1)
    // could also be NUM_LEDS_Y, whichever is greater
    {
      Serial.printf("\nResetting rippleCounter1...");
      rippleCounter1 = 0;
    } else {
      Serial.printf("\nGenerating frame for button 1...");
      // generates 1 frame
      rippleEffect(255, 0, 0, 4, 4, rippleCounter1, prevLeds1, width);
      if (rippleCounter1 == 0) {  // fixing bug like this instead of fixing my buggy rippleEffect() ;*
        leds[XY(15, 15)] = CRGB(0, 0, 0);
      }
      rippleCounter1++;
    }
  }

  button1_status = digitalRead(INPUT_BTN_NE);
  button2_status = digitalRead(INPUT_BTN_NW);
  button3_status = digitalRead(INPUT_BTN_SE);
  button4_status = digitalRead(INPUT_BTN_SW);

  if (button2_status) {
    if (rippleCounter2 == NUM_LEDS_X + width + 1) {
      Serial.printf("\nResetting rippleCounter2...");
      rippleCounter2 = 0;
    } else {
      Serial.printf("\nGenerating frame for button 2...");
      // generates 1 frame
      rippleEffect(0, 255, 0, 11, 11, rippleCounter2, prevLeds2, width);
      if (rippleCounter2 == 0) {  // fixing bug like this instead of fixing my buggy rippleEffect() ;*
        leds[XY(0, 0)] = CRGB(0, 0, 0);
      }
      rippleCounter2++;
    }
  }

  button1_status = digitalRead(INPUT_BTN_NE);
  button2_status = digitalRead(INPUT_BTN_NW);
  button3_status = digitalRead(INPUT_BTN_SE);
  button4_status = digitalRead(INPUT_BTN_SW);

  if (button3_status) {
    if (rippleCounter3 == NUM_LEDS_X + width + 1) {
      Serial.printf("\nResetting rippleCounter3...");
      rippleCounter3 = 0;
    } else {
      Serial.printf("\nGenerating frame for button 3...");
      // generates 1 frame
      rippleEffect(0, 0, 255, 11, 4, rippleCounter3, prevLeds3, width);
      if (rippleCounter3 == 0) {  // fixing bug like this instead of fixing my buggy rippleEffect() ;*
        leds[XY(0, 15)] = CRGB(0, 0, 0);
      }
      rippleCounter3++;
      Serial.printf(" rippleCounter3 = %d", rippleCounter3);
    }
  }

  button1_status = digitalRead(INPUT_BTN_NE);
  button2_status = digitalRead(INPUT_BTN_NW);
  button3_status = digitalRead(INPUT_BTN_SE);
  button4_status = digitalRead(INPUT_BTN_SW);

  if (button4_status) {
    Serial.printf("\nButton 4 pressed...");
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    bool running = true;
    while (running == true) {
      button1_status = digitalRead(INPUT_BTN_NE);
      button2_status = digitalRead(INPUT_BTN_NW);
      button3_status = digitalRead(INPUT_BTN_SE);

      if (button1_status || button2_status || button3_status) {
        running = false;
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        break;
      }

      uint32_t ms = millis();
      int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / kMatrixWidth));
      int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / kMatrixHeight));
      DrawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
      if (ms < 5000) {
        FastLED.setBrightness(scale8(MAX_BRIGHTNESS, (ms * 256) / 5000));
      } else {
        FastLED.setBrightness(MAX_BRIGHTNESS);
      }
      FastLED.show();
    }

    // reset the ripples after leaving rainbow effect
    rippleCounter1 = 0;
    rippleCounter2 = 0;
    rippleCounter3 = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
      prevLeds1[i] = 0;
      prevLeds2[i] = 0;
      prevLeds3[i] = 0;
    }

    // if (rippleCounter4 == NUM_LEDS_X + width + 1)
    // {
    //   Serial.printf("\nResetting rippleCounter4...");
    //   rippleCounter4 = 0;
    // }
    // else
    // {
    //   Serial.printf("\nGenerating frame for button 4...");
    //   // generates 1 frame
    //   rippleEffect(255, 255, 255, 4, 11, rippleCounter4, prevLeds4, width);
    //   if (rippleCounter4 == 0)
    //   { // fixing bug like this instead of fixing my buggy rippleEffect() ;*
    //     leds[XY(15, 0)] = CRGB(0, 0, 0);
    //   }
    //   rippleCounter4++;
    // }
  }

  FastLED.show();
  delay(75);

  // static int rippleCounter2 = 0;

  // // Clear the LED array before each frame
  // fill_solid(leds, NUM_LEDS, CRGB::Black);

  // // Create frame for first ripple effect
  // rippleEffect(28, 194, 255, NUM_LEDS_X / 2, NUM_LEDS_Y / 4, rippleCounter1); // cyan ripple

  // // Create frame for second ripple effect
  // rippleEffect(124, 25, 255, NUM_LEDS_X / 2, NUM_LEDS_Y - NUM_LEDS_Y / 4 - 1, rippleCounter2); // purple ripple

  // // Display the frames simultaneously
  // FastLED.show();

  // // Increment the counters for the ripple effects (for progressing to next frame of ripple)
  // rippleCounter1++;
  // rippleCounter2++;

  // // Adjust delay for speed of the ripple effect
  // delay(75);
}
#endif

/**
 * Draws a single frame of the rainbow effect
 */
void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
  uint8_t lineStartHue = startHue8;
  for (uint8_t y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    uint8_t pixelHue = lineStartHue;
    for (uint8_t x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[XY(x, y)] = CHSV(pixelHue, 255, 255);
    }
  }
}

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
void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y, int rippleCounter, int prevLeds[], int width) {
  uint8_t maxDistance = 19;  // max(NUM_LEDS_X, NUM_LEDS_Y);

  // Iterate through the LED matrix
  for (uint8_t x = 0; x < NUM_LEDS_X; x++) {
    for (uint8_t y = 0; y < NUM_LEDS_Y; y++) {
      // Calculate distance and ripple distance
      uint8_t distance = calculateDistance(center_x, center_y, x, y);
      uint8_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance);
      uint8_t brightness;
      uint16_t xy_val = XY(x, y);

      if (prevLeds[xy_val] == 1) {
        prevLeds[xy_val] = 0;
        CRGB updatedColor = CRGB(leds[xy_val].r - (r * MAX_BRIGHTNESS), leds[xy_val].g - (g * MAX_BRIGHTNESS), leds[xy_val].b - (b * MAX_BRIGHTNESS));
        leds[xy_val] = updatedColor;
        continue;
      }

      // Determine brightness based on distance from center and rippleCounter
      if (rippleDistance <= width) {
        brightness = MAX_BRIGHTNESS;
        prevLeds[xy_val] = 1;
      } else {
        brightness = 0;  // Dim brightness value outside the ripple's ring
        prevLeds[xy_val] = 0;
      }

      // Create a CRGB object with the calculated color and brightness
      CRGB newColor = CRGB(r * brightness, g * brightness, b * brightness);

      // Add the newColor to the existing LED color using blend function
      leds[xy_val] += newColor;
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
uint16_t XY(uint8_t x, uint8_t y) {
  int i;

  if (kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * kMatrixWidth) + x;
    } else {
      i = kMatrixHeight * (kMatrixWidth - (x + 1)) + y;
    }
  }

  if (kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if (y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (kMatrixWidth - 1) - x;
        i = (y * kMatrixWidth) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * kMatrixWidth) + x;
      }
    } else {  // vertical positioning
      if (x & 0x01) {
        i = kMatrixHeight * (kMatrixWidth - (x + 1)) + y;
      } else {
        i = kMatrixHeight * (kMatrixWidth - x) - (y + 1);
      }
    }
  };

  return i;
}

/**
 * Makes sure the specified point is in bounds before calculating its (x, y) position.
 */
uint16_t XYsafe(uint8_t x, uint8_t y) {
  if (x >= kMatrixWidth)
    return -1;
  if (y >= kMatrixHeight)
    return -1;
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
