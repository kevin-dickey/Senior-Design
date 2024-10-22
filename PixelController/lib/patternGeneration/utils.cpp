#include <cmath>

#include "FastLED.h"
#include "utils.h"

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
 * Introduces variability/noise to the provided LEDs by a specified amount (+- noiseLevel).
 * Needs testing to find a good value for the noiseLevel.
 * 
 * Skips adding noise to black pixels, skips updating the pixel if the result is white (doesn't actually for some reason).
 * 
 * Doesn't really work. (breaks when noiseLevel is >1..., not sure it even works with a value of 1 lol)
 */
void addNoise(CRGB leds[], int numLeds, uint8_t noiseLevel) {
    uint32_t currentTime = millis();
    for (int i = 0; i < numLeds; i++) {
        if (leds[i].r == 0 && leds[i].g == 0 && leds[i].b == 0) {
            continue; // skip if the pixel is off
        }

        // another approach... still doesn't work properly :(
        uint16_t seed = (i * 37) + currentTime;
        random16_set_seed(seed);
        CHSV hsv = rgb2hsv_approximate(leds[i]);
        hsv.hue = (hsv.hue + random8(-noiseLevel, noiseLevel)) % 256;
        // hsv.value = (hsv.value + random(-noiseLevel, noiseLevel)) % 256;
        leds[i] = hsv2rgb_spectrum(hsv);

        


        // trying different approach -- works "better", still doesn't really work though.
        // CHSV hsv = rgb2hsv_approximate(leds[i]);
        // int8_t newHue = hsv.hue + (int8_t)(sin16(currentTime + i + 100) / 32768.0 * noiseLevel);
        // constrain(newHue, 0, 255);        
        // leds[i] = hsv2rgb_spectrum(hsv);

        // compiler optimizations should fix me calling the same thing multiple times (i'm lazy)
        // int noise = random8(-noiseLevel, noiseLevel);
        // if ((constrain(leds[i].r + noise, 0, 255) == 255 && constrain(leds[i].g + noise, 0, 255) == 255 && constrain(leds[i].b + noise, 0, 255) == 255) 
        //  || (constrain(leds[i].r + noise, 0, 255) == constrain(leds[i].g + noise, 0, 255) == constrain(leds[i].b + noise, 0, 255))
        //  || (std::abs(leds[i].r - leds[i].g) <= std::abs(noiseLevel) && std::abs(leds[i].g - leds[i].b) <= std::abs(noiseLevel) && (std::abs(leds[i].r - leds[i].b) <= std::abs(noiseLevel)))) {
        //     continue; // skip if result is white (or very close to it)
        // }

        // leds[i].r = constrain(leds[i].r + noise, 0, 255);
        // leds[i].g = constrain(leds[i].g + noise, 0, 255);
        // leds[i].b = constrain(leds[i].b + noise, 0, 255);
    }
}

/**
 * Rotates the given array of LEDs either clockwise or counter clockwise
 * in iterations of 90 degrees.
 * 
 * Might be good to implement a function to change how XY() indexes (permanent rotation instead of singular frame-based rotation)
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
