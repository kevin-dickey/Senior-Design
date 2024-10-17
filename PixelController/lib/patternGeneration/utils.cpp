#include <cmath>

#include "FastLED.h"
#include "utils.h"

enum RotationDirection {
    Clockwise,
    CounterClockwise
};


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

/**
 * Introduces variability/noise to the provided LEDs by a specified amount.
 * Needs testing to find a good value for the noiseLevel.
 */
void addNoise(CRGB leds[], int numLeds, uint8_t noiseLevel) {
    for (int i = 0; i < numLeds; i++) {
        int noiseR = random8(-noiseLevel, noiseLevel + 1);
        int noiseG = random8(-noiseLevel, noiseLevel + 1);
        int noiseB = random8(-noiseLevel, noiseLevel + 1);

        leds[i].r = constrain(leds[i].r + noiseR, 0, 255);
        leds[i].g = constrain(leds[i].r + noiseR, 0, 255);
        leds[i].b = constrain(leds[i].r + noiseR, 0, 255);
    }
    FastLED.show();
}

/**
 * Rotates the given array of LEDs either clockwise or counter clockwise
 * in iterations of 90 degrees.
 */
void rotateLeds(CRGB leds[], RotationDirection dir, int degrees, int ledsHeight, int ledsWidth) {
    if (degrees % 90 != 0) {
        Serial.println("Can only rotate the degrees in increments of 90 degrees.");
        return;
    }

    // minimize the number of rotations
    degrees = degrees % 360;
    while (degrees < 0) {
        degrees += 360;
    }

    // further minimize the number of rotations
    if (dir == Clockwise && degrees == 270) {
        dir = CounterClockwise;
        degrees = 90;
    } else if (dir == CounterClockwise && degrees == 270) {
        dir = Clockwise;
        degrees = 90;
    }

    int increments = degrees / 90;

    // dummy check
    if (degrees == 360) {
        return;
    } else if (degrees == 180) {
        rotate180(leds, ledsHeight, ledsWidth);
    }

    // rotate time
    for (int i = 0; i < increments; i++) {
        if (dir == Clockwise) {
            rotate90Clockwise(leds, ledsHeight, ledsWidth);
        } else if (dir == CounterClockwise) {  
            rotate90CounterClockwise(leds, ledsHeight, ledsWidth);
        } else {
            Serial.println("Unrecognized rotation direction (i.e. not clockwise or counterclockwise)");
        }
    }

    FastLED.show();
}

void rotate90Clockwise(CRGB leds[], uint16_t height, uint16_t width) {
    CRGB temp[width * height];
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint16_t newIndex = XY(height - 1 - y, x);
            uint16_t oldIndex = XY(x, y);
            temp[newIndex] = leds[oldIndex];
        }
    }
    // Copy back the rotated values to the original leds array
    memcpy(leds, temp, sizeof(temp));
}

void rotate90CounterClockwise(CRGB leds[],  uint16_t height, uint16_t width) {
    CRGB temp[width * height];
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint16_t newIndex = XY(y, width - 1 - x);
            uint16_t oldIndex = XY(x, y);
            temp[newIndex] = leds[oldIndex];
        }
    }
    // Copy back the rotated values to the original leds array
    memcpy(leds, temp, sizeof(temp));
}

void rotate180(CRGB leds[], uint16_t height, uint16_t width) {
    CRGB temp[width * height];
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint16_t newIndex = XY(width - 1 - x, height - 1 - y);
            uint16_t oldIndex = XY(x, y);
            temp[newIndex] = leds[oldIndex];
        }
    }
    // Copy back the rotated values to the original leds array
    memcpy(leds, temp, sizeof(temp));
}
