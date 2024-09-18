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
void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y, int rippleCounter)
{
  uint8_t maxDistance = max(NUM_LEDS_X, NUM_LEDS_Y);

  // Iterate through the LED matrix
  for (uint8_t x = 0; x < NUM_LEDS_X; x++)
  {
    for (uint8_t y = 0; y < NUM_LEDS_Y; y++)
    {
      // Calculate distance and ripple distance
      uint8_t distance =  calculateDistance(center_x, center_y, x, y);
      uint8_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance + 1);
      uint8_t brightness;

      // Determine brightness based on distance from center and rippleCounter
      if (rippleDistance <= 1)
      {
        brightness = MAX_BRIGHTNESS;
      }
      else
      {
        brightness = 0; // Dim brightness value outside the ripple's ring
      }

      // Create a CRGB object with the calculated color and brightness
      CRGB newColor = CRGB(r * brightness / MAX_BRIGHTNESS, g * brightness / MAX_BRIGHTNESS, b * brightness / MAX_BRIGHTNESS);

      // Add the newColor to the existing LED color using blend function
      leds[XY(x, y)] += newColor;
    }
  }
}