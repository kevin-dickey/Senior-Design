#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>


#define NUM_LEDS  60
#define LED_PIN   13

CRGB leds[NUM_LEDS] = {0};  // Initialize the array of leds

// FramesPerSecond
//
// Tracks a weighted average to smooth out the values that it calcs as the simple reciprocal
// of the amount of time taken specified by the caller. So 1/3 of a second is 3 fps, and it
// will take up to 10 frames or so to stabilize on that value.

double FramesPerSecond(double seconds) {
  static double framesPerSecond;
  framesPerSecond = (framesPerSecond * .9) + (1.0 / seconds * .1);
  return framesPerSecond;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("ESP32 Startup");

  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);    // Add our strip to the FastLED library
  FastLED.setBrightness(16);

}

void loop() {
  bool bLED = 0;
  double fps = 0;

  uint8_t hue = 0;
  const uint8_t deltaHue = 8;
  const uint8_t hueDensity = 8;

  for (;;)
  {
    bLED = !bLED;
    digitalWrite(LED_BUILTIN, bLED);

    double dStart = millis() / 1000.0;

    for (int i = 0; i < NUM_LEDS; i += 5)
    {
      leds[i] = CHSV(hue + (i * hueDensity), 255, 255);
    }
    // fill_rainbow(leds, NUM_LEDS, hue += hueDensity, deltaHue);

    FastLED.show();
    Serial.println(fps);
    delay(100);
  
    double dEnd = millis() / 1000.0;
    fps = FramesPerSecond(dEnd - dStart);
  }
}