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
void rippleEffect(CRGB *frame_data,
                   int num_leds_x, int num_leds_y, 
                   int r, int g, int b, 
                   uint8_t center_x, uint8_t center_y, 
                   int rippleCounter, int prevLeds[], 
                   int width)
{
    uint16_t maxDistance = max(num_leds_x, num_leds_y);
    // uint16_t total_leds = num_leds[0] * num_leds[1];

    // Shallow copy of the LED array
    // auto *temp = new CRGB[total_leds];

    // std::cout << "Alloced" << std::endl;

    // for (int i = 0; i < total_leds; i++)
    // {
    //     temp[i] = frame_data[i];
    // }

    if (prevLeds == NULL)
    {
        fill_solid(frame_data, num_leds_x * num_leds_y, CRGB::Black);
    }

    std::cout << "Filled" << std::endl;

    // Iterate through the LED matrix
    for (uint16_t x = 0; x < num_leds_x; x++)
    {
        for (uint16_t y = 0; y < num_leds_y; y++)
        {
            // Calculate distance and ripple distance
            uint16_t distance = calculateDistance(center_x, center_y, x, y);
            uint16_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance);
            uint16_t xy_val = XY(x, y);
            uint8_t brightness;

            // if (prevLeds[xy_val] == 1) { // too memory intensive for esp32 for now, instead just fully clearing. ripple won't work w/ every effect so keep in mind!
            //     prevLeds[xy_val] = 0;
            //     CRGB updatedColor = CRGB(leds[xy_val].r - r, leds[xy_val].g - g, leds[xy_val].b - b);
            //     leds[xy_val] = updatedColor;
            //     continue;
            // }

            // Determine brightness based on distance from center and rippleCounter
            if (rippleDistance <= width)
            {
                brightness = 0xFF;
                // prevLeds[xy_val] = 1;
            }
            else
            {
                brightness = 0; // Dim brightness value outside the ripple's ring
                // prevLeds[xy_val] = 0;
            }

            // Create a CRGB object with the calculated color and brightness
            CRGB newColor = CRGB(r * brightness, g * brightness, b * brightness);

            // blend the newColor to the existing LED color
            // temp[xy_val] += newColor;
            frame_data[xy_val] += newColor;
        }
    }
    std::cout << "Patterned" << std::endl;
}