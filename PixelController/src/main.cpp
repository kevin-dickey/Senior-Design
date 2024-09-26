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

enum ShiftDirection {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

void fadeToBlack(int duration);
void fadeToBrightness(int duration, int targetBrightness);

void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y, int rippleCounter, int prevLeds[], int width);
uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter);  // depricated function
uint16_t XY(uint8_t x, uint8_t y);
uint16_t XYsafe(uint8_t x, uint8_t y);
void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8);  // draws rainbow frame

void parseBitmapData(const char *hexData);
CRGB hexToCRGB(const char *hex);

void shiftLeds(CRGB leds[], ShiftDirection direction);

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

const char *pumpkin =
    "ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff "
    "ffffff ffffff ffffff ffffff ffffff ffffff ffffff 74401f ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff "
    "ffffff ffffff ffffff ffffff ffffff ffffff ffffff 764322 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff "
    "ffffff ffffff ffffff ffffff ff7f15 ff7f15 ff7f15 80431c 74401f ff7f15 ff7f15 ff7f15 ff8017 ffffff ffffff ffffff "
    "ffffff ffffff ffffff ff7f15 ff7f15 ff8c2c ff7f15 ff7f15 ff8c2c ff7f15 ff7f15 ff7f15 ff7f15 ff8623 ffffff ffffff "
    "ffffff ffffff ff7f15 de741d ff8c2c 2d1200 2d1200 ff7f15 ff7f15 2d1200 2d1200 ff8c2c f77b15 ff8723 ffffff ffffff "
    "ffffff ff8118 ff7f15 ff8c2c 2d1200 863b20 863b20 2d1200 69320a 863b20 863b20 2d1200 ff8118 ff7f15 ff8520 ffffff "
    "ffffff ff7f15 ff7f15 ff7f15 ff7f15 db731d ff7f15 ff7f15 ff7f15 ff8c2c ff7f15 ff7f15 ff8c2c ff7f15 ff8c2c ffffff "
    "ffffff ff7f15 ff7f15 ff7f15 ff7f15 f27914 ff7f15 2c1100 2d1200 ff8c2c ff7f15 ff7f15 ff8c2c ff7f15 ff8c2c ffffff "
    "ffffff ff7f15 ff7f15 863b20 ff7f15 f27914 ff7f15 ff7f15 ff7f15 ff8c2c f27914 2d1200 863b20 ff7f15 ff8c2c ffffff "
    "ffffff ff7f15 ff7f15 ff7f15 2a1000 f27914 2d1200 2d1200 2d1200 2d1200 f27914 2d1200 ff7f15 ff7f15 ff7f15 ffffff "
    "ffffff ff7f15 ff7f15 ff7f15 863b20 2d1200 2d1200 2d1200 2d1200 2d1200 2d1200 863b20 cf7022 ff7f15 ffffff ffffff "
    "ffffff ffffff ff7f15 f27914 ff7f15 863b20 2d1200 2d1200 2d1200 2d1200 863b20 ff7f15 f27914 ff7f15 ffffff ffffff "
    "ffffff ffffff ffffff ff7f15 f27914 f67a14 f27914 f27914 f27914 cc7024 f27914 dc731d ff7f15 ffffff ffffff ffffff "
    "ffffff ffffff ffffff ffffff f27914 ed7816 f27914 f27914 f27914 f27914 ffffff ffffff ffffff ffffff ffffff ffffff "
    "ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff";

/**
 * MARK: Setup
 */
void setup() {
    Serial.begin(115200);                                                                          // for setting up stuff to print to serial monitor
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);  // setup the LEDs & LED pin for the esp32
    FastLED.setBrightness(MAX_BRIGHTNESS);                                                         // set the max brightness for the LEDs
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();

    parseBitmapData(pumpkin);  // currently sets all of the leds on the matrix to be the pumpkin stuff
}

/**
 * MARK: Looping
 */
void loop() {
    shiftLeds(leds, RIGHT);

    delay(50);
}
#endif

void shiftLeds(CRGB leds[], ShiftDirection direction) {
    CRGB temp[NUM_LEDS_X * NUM_LEDS_Y];

    // Copy current state to temp array
    for (int i = 0; i < NUM_LEDS_X * NUM_LEDS_Y; i++) {
        temp[i] = leds[i];
    }

    switch (direction) {
        case RIGHT:  // if you look closely right and left might look flipped, and you're right!
                     // don't ask me why, it just works :)
            for (int y = 0; y < NUM_LEDS_Y; y++) {
                for (int x = 0; x < NUM_LEDS_X; x++) {
                    int newX = (x - 1 + NUM_LEDS_X) % NUM_LEDS_X;
                    leds[XY(newX, y)] = temp[XY(x, y)];
                }
            }
            break;

        case LEFT:
            for (int y = 0; y < NUM_LEDS_Y; y++) {
                for (int x = 0; x < NUM_LEDS_X; x++) {
                    int newX = (x + 1) % NUM_LEDS_X;
                    leds[XY(newX, y)] = temp[XY(x, y)];
                }
            }
            break;

        case UP:
            for (int x = 0; x < NUM_LEDS_X; x++) {
                for (int y = 0; y < NUM_LEDS_Y; y++) {
                    int newY = (y - 1 + NUM_LEDS_Y) % NUM_LEDS_Y;
                    leds[XY(x, newY)] = temp[XY(x, y)];
                }
            }
            break;

        case DOWN:
            for (int x = 0; x < NUM_LEDS_X; x++) {
                for (int y = 0; y < NUM_LEDS_Y; y++) {
                    int newY = (y + 1) % NUM_LEDS_Y;
                    leds[XY(x, newY)] = temp[XY(x, y)];
                }
            }
            break;
    }

    FastLED.show();  // Update the LED display
}

/**
 * duration is given in seconds.
 *
 * Uses FastLED's builtin for setting brightness, so it'll modify every LED.
 *
 * If you want to use it on a specific subset of LEDs, will have to provide
 * that subset as well as somehow keeping track of what the LEDs previously were.
 */
void fadeToBlack(int duration) {
    uint8_t initialBrightness = FastLED.getBrightness();
    if (initialBrightness == 0) return;

    int updatesPerSec = initialBrightness / duration;

    for (int i = initialBrightness; i > 0; i--) {
        FastLED.setBrightness(i);
        FastLED.show();
        delay(1000 / updatesPerSec);
    }

    // Ensure the brightness is fully set to 0 at the end
    FastLED.setBrightness(0);
    FastLED.show();
}

/**
 * Slowly and "smoothly" transitions the brightness of all LEDs to the targetBrightness over the given duration.
 *
 * duration is given in seconds.
 * targetBrightness should generally not be set beyond 32 (64 likely maximum for safety/consistent power delivery)
 *
 * Uses FastLED's builtin for setting brightness, so it'll modify every LED.
 *
 * If you want to use it on a specific subset of LEDs, will have to provide
 * that subset as well as somehow keeping track of what the LEDs previously were.
 */
void fadeToBrightness(int duration, int targetBrightness) {
    uint8_t curBrightness = FastLED.getBrightness();
    if (curBrightness == targetBrightness) return;

    int updatesPerSec;
    if (targetBrightness > curBrightness) {  // increase brightness to target
        updatesPerSec = (targetBrightness - curBrightness) / duration;
        for (int i = curBrightness; i < targetBrightness; i++) {
            FastLED.setBrightness(i);
            FastLED.show();
            delay(1000 / updatesPerSec);
        }
    } else {  // decrease brightness to target
        updatesPerSec = (curBrightness - targetBrightness) / duration;
        for (int i = curBrightness; i > targetBrightness; i--) {
            FastLED.setBrightness(i);
            FastLED.show();
            delay(1000 / updatesPerSec);
        }
    }

    FastLED.setBrightness(targetBrightness);  // just in case it doesnt fully work lol
    FastLED.show();
}

// Function to convert a 6-character hex string to CRGB
CRGB hexToCRGB(const char *hex) {
    uint8_t r = strtol(std::string(hex, 2).c_str(), NULL, 16);
    uint8_t g = strtol(std::string(hex + 2, 2).c_str(), NULL, 16);
    uint8_t b = strtol(std::string(hex + 4, 2).c_str(), NULL, 16);
    return CRGB(r, g, b);
}

// Function to parse the bitmap data from a hex string
void parseBitmapData(const char *hexData) {
    int index = 0;
    while (*hexData) {
        // Skip spaces
        if (*hexData == ' ') {
            hexData++;
            continue;
        }

        // Convert the next 6 characters to CRGB and store in the leds array
        if (index < NUM_LEDS) {
            leds[index] = hexToCRGB(hexData);
            hexData += 6;  // Move to the next color
            index++;
        } else {
            break;  // Avoid exceeding the array size
        }
    }
}

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
