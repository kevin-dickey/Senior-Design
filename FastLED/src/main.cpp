
#include <FastLED.h>
#include <iostream>

#define LED_PIN     22
#define NUM_LEDS_X  16
#define NUM_LEDS_Y  16
#define NUM_LEDS    NUM_LEDS_X * NUM_LEDS_Y
#define MAX_BRIGHTNESS  16
// #define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define CHIPSET     WS2811

void rippleEffect();
uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter);
uint16_t XY( uint8_t x, uint8_t y);
uint16_t XYsafe( uint8_t x, uint8_t y);

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(MAX_BRIGHTNESS);
}

void loop() {
  rippleEffect();
  // std::cout << "Done with rippleEffect()" << std::endl; // idk how to get this to even show up in console lol
  FastLED.show();
  delay(125);  // adjust delay for speed of the ripple effect  
}

void rippleEffect() {
    static uint8_t rippleCounter = 0;
    uint8_t center_x = NUM_LEDS_X / 2;
    uint8_t center_y = NUM_LEDS_Y / 2;
    uint8_t maxDistance = max(NUM_LEDS_X / 2, NUM_LEDS_Y / 2);

    for (uint8_t x = 0; x < NUM_LEDS_X; x++) {
        for (uint8_t y = 0; y < NUM_LEDS_Y; y++) {
            uint8_t distance = calculateDistance(center_x, center_y, x, y);
            uint8_t rippleDistance = (rippleCounter + (maxDistance - distance)) % (maxDistance + 1);
            uint8_t brightness;

            // Determine brightness based on distance from center and rippleCounter
            if (rippleDistance <= 2) {
                // If the pixel is within the ring
                brightness = MAX_BRIGHTNESS - rippleDistance * 85; // Gradually decrease brightness towards the edge of the ring
            } else {
                // If the pixel is outside the ring
                brightness = 0; // Dim brightness value
            }

            leds[XY(x, y)] = CRGB(0, 0, brightness);  // Adjust color as needed
        }
    }

    rippleCounter++;
}

// void rippleEffect() {
//   static int rippleCounter = 0;
//   uint8_t center_x = NUM_LEDS_X / 2;
//   uint8_t center_y = NUM_LEDS_Y / 2;

//   for (int x = 0; x < NUM_LEDS; x++) {
//     for (int y = 0; y < NUM_LEDS; y++) {
//       uint8_t distance = calculateDistance(center_x, center_y, x, y);
//       uint8_t brightness = scaleBrightness(distance, rippleCounter);
//       leds[XYsafe(x, y)] = CRGB(0, 0, brightness); // just different brightnesses of blue
//     }
//   }

//   rippleCounter++;
// }

uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  float dx = abs(x2 - x1);
  float dy = abs(y2 - y1);
  return sqrt(dx * dx + dy * dy);
}

uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter) {
  uint8_t delta = abs(rippleCounter - distance);
  uint8_t maxDistance = NUM_LEDS / 2;
  uint8_t brightness = map(delta, 0, maxDistance, 0, MAX_BRIGHTNESS);
  // return (brightness <= 0) ? 0 : (brightness > 2) ? 2 : brightness; // ensures 0 <= brightness <= 16
  return brightness > MAX_BRIGHTNESS ? 0 : brightness;
}


// #include <FastLED.h>

// #define LED_PIN  22

// #define COLOR_ORDER GRB
// #define CHIPSET     WS2811

// #define BRIGHTNESS 8 // max is 64 (?) maybe 256 idk
// // if you set the brightness to 1, it only shows red stripes! DO NOT DO THIS WITHOUT A POWER SUPPLY CONNECTED (byebye usb)
// // even brightness of 2 is able to show the colors quite well, but probably missing some saturation?

// Params for width and height
const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;
const bool    kMatrixVertical = false;

uint16_t XY( uint8_t x, uint8_t y)
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
  }
  
  return i;
}

// #define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
// CRGB* const leds( leds_plus_safety_pixel + 1);

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}


// // Demo that USES "XY" follows code below
// void DrawOneFrame( uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
// {
//   uint8_t lineStartHue = startHue8;
//   for( uint8_t y = 0; y < kMatrixHeight; y++) {
//     lineStartHue += yHueDelta8;
//     uint8_t pixelHue = lineStartHue;      
//     for( uint8_t x = 0; x < kMatrixWidth; x++) {
//       pixelHue += xHueDelta8;
//       leds[ XY(x, y)]  = CHSV( pixelHue, 255, 255);
//     }
//   }
// }


// void loop()
// {
//     uint32_t ms = millis();
//     int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
//     int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
//     DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);

//     // maybe setting a random brightness for each pixel could be cool
    


//     if( ms < 5000 ) {
//       FastLED.setBrightness( scale8( BRIGHTNESS, (ms * 256) / 5000));
//       // FastLED.setBrightness( scale8(random(16, 64), (ms * 256) / 5000)); // thought this might look cool but its just seizure mode (its for brightness of all pixels)
//     } else {
//       FastLED.setBrightness(BRIGHTNESS);
//       // FastLED.setBrightness(random(16, 64)); // thought this might look cool but its just seizure mode (its for brightness of all pixels)
//     }
//     FastLED.show();
//     // delay(1000);
// }


// void setup() {
//   FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
//   FastLED.setBrightness( BRIGHTNESS );
//   // randomSeed(analogRead(0));
// }
