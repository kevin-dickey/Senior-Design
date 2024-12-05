#include "rippleEffect.h"

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
CRGB* rippleEffect(CRGB *frame_data, int *num_leds, int r, int g, int b, uint8_t center_x, uint8_t center_y, int rippleCounter, int prevLeds[], int width) {
    uint8_t maxDistance = 19;  // max(NUM_LEDS_X, NUM_LEDS_Y);
    uint16_t total_leds = num_leds[0] * num_leds[1];
    
    // Shallow copy of the LED array
    auto *temp = new CRGB[total_leds];

    for (int i = 0; i < total_leds; i++) {
        temp[i] = frame_data[i];
    }

    // Iterate through the LED matrix
    for (uint8_t x = 0; x < num_leds[0]; x++) {
        for (uint8_t y = 0; y < num_leds[1]; y++) {
            // Calculate distance and ripple distance
            uint8_t distance = calculateDistance(center_x, center_y, x, y);
            uint8_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance);
            uint8_t brightness;
            uint16_t xy_val = XY(x, y);

            if (prevLeds[xy_val] == 1) {
                prevLeds[xy_val] = 0;
                CRGB updatedColor = CRGB(frame_data[xy_val].r - r, frame_data[xy_val].g - g, frame_data[xy_val].b - b);
                frame_data[xy_val] = updatedColor;
                continue;
            }

            // Determine brightness based on distance from center and rippleCounter
            if (rippleDistance <= width) {
                brightness = 255;
                prevLeds[xy_val] = 1;
            } else {
                brightness = 0;  // Dim brightness value outside the ripple's ring
                prevLeds[xy_val] = 0;
            }

            // Create a CRGB object with the calculated color and brightness
            CRGB newColor = CRGB(r * brightness, g * brightness, b * brightness);

            // blend the newColor to the existing LED color
            temp[xy_val] += newColor;
        }
    }
    return temp;
}