#include "effects.h"

// current_millis isn't a LiveData (see android studio), so should be ok to pass (don't need to though, it's relatively global)
void drawRainbow(unsigned long current_millis) {
    int32_t yHueDelta32 = ((int32_t)cos16(current_millis * (27 / 1)) * (350 / kMatrixWidth));
    int32_t xHueDelta32 = ((int32_t)cos16(current_millis * (39 / 1)) * (310 / kMatrixHeight));
    DrawOneFrameReducedBright(current_millis / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
}

// Function to load an 8x8 bitmap from a hex string
// startX and startY specify the position of the top-right of the bitmap to load in
void loadHexBitmap(CRGB *leds, const char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth) {
    for (uint8_t y = 0; y < bitmapHeight; y++) {
        for (uint8_t x = 0; x < bitmapWidth; x++) {
            // Calculate the position in the string
            int index = (y * bitmapWidth + x) * 7;  // 6 for color + 1 for space

            // Extract the hex color (6 characters)
            char hexColor[7];  // 6 for color + 1 for null terminator
            strncpy(hexColor, &bitmap[index], 6);
            hexColor[6] = '\0';  // Null-terminate the string

            // Only load if within bounds
            if (startX + x < kMatrixWidth && startY + y < kMatrixHeight) {
                CRGB color = hexToCRGB(hexColor);
                if (color != (CRGB::Black)) {  // black color is interpreted as intending to be transparent
                    leds[XY(startX + x, startY + y)] = color;
                }
            }
        }
    }
    FastLED.show();
}

// cant remember if these were moved somewhere else in main, keepin here will remove if nedded
void shiftLeds(CRGB leds[], int leds_x, int leds_y, ShiftDirection direction) {
    CRGB temp[leds_x * leds_y];

    // Copy current state to temp array
    for (int i = 0; i < leds_x * leds_y; i++) {
        temp[i] = leds[i];
    }

    switch (direction) {
        case RIGHT:  // if you look closely right and left might look flipped, and you're right!
                     // don't ask me why, it just works :)
            for (int y = 0; y < leds_y; y++) {
                for (int x = 0; x < leds_x; x++) {
                    int newX = (x - 1 + leds_x) % leds_x;
                    leds[XY(newX, y)] = temp[XY(x, y)];
                }
            }
            break;

        case LEFT:
            for (int y = 0; y < leds_y; y++) {
                for (int x = 0; x < leds_x; x++) {
                    int newX = (x + 1) % leds_x;
                    leds[XY(newX, y)] = temp[XY(x, y)];
                }
            }
            break;

        case UP:
            for (int x = 0; x < leds_x; x++) {
                for (int y = 0; y < leds_y; y++) {
                    int newY = (y - 1 + leds_y) % leds_y;
                    leds[XY(x, newY)] = temp[XY(x, y)];
                }
            }
            break;

        case DOWN:
            for (int x = 0; x < leds_x; x++) {
                for (int y = 0; y < leds_y; y++) {
                    int newY = (y + 1) % leds_y;
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
    if (initialBrightness == 0)
        return;

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
    if (curBrightness == targetBrightness)
        return;

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
