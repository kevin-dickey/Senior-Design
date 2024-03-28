
#include <FastLED.h>

#define LED_PIN     22
#define NUM_LEDS    256
#define BRIGHTNESS  8
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

void rippleEffect();
int calculateDistance(int x1, int y1, int x2, int y2);
int scaleBrightness(int distance, int rippleCounter);


CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  rippleEffect();
  FastLED.show();
  delay(50);  // Adjust delay for speed of the ripple effect
}

void rippleEffect() {
  static int rippleCounter = 0;
  int center_x = NUM_LEDS / 2;
  int center_y = NUM_LEDS / 2;

  for (int x = 0; x < NUM_LEDS; x++) {
    for (int y = 0; y < NUM_LEDS; y++) {
      int distance = calculateDistance(center_x, center_y, x, y);
      int brightness = scaleBrightness(distance, rippleCounter);
      leds[ XY(x, y)] = CRGB(brightness, 0, 0);  // just red
    }
  }

  rippleCounter++;
}

int calculateDistance(int x1, int y1, int x2, int y2) {
  float dx = abs(x2 - x1);
  float dy = abs(y2 - y1);
  return sqrt(dx * dx + dy * dy);
}

int scaleBrightness(int distance, int rippleCounter) {
  int delta = abs(rippleCounter - distance);
  int maxDistance = NUM_LEDS / 2;
  int brightness = map(delta, 0, maxDistance, 255, 0);
  return brightness > 255 ? 0 : brightness; // Ensures brightness does not exceed 255 or go below 0
}


// #include <FastLED.h>

// #define LED_PIN  22

// #define COLOR_ORDER GRB
// #define CHIPSET     WS2811

// #define BRIGHTNESS 8 // max is 64 (?) maybe 256 idk
// // if you set the brightness to 1, it only shows red stripes! DO NOT DO THIS WITHOUT A POWER SUPPLY CONNECTED (byebye usb)
// // even brightness of 2 is able to show the colors quite well, but probably missing some saturation?

// // Params for width and height
// const uint8_t kMatrixWidth = 16;
// const uint8_t kMatrixHeight = 16;

// // Param for different pixel layouts
// const bool    kMatrixSerpentineLayout = true;
// const bool    kMatrixVertical = false;

// uint16_t XY( uint8_t x, uint8_t y)
// {
//   uint16_t i;
  
//   if( kMatrixSerpentineLayout == false) {
//     if (kMatrixVertical == false) {
//       i = (y * kMatrixWidth) + x;
//     } else {
//       i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
//     }
//   }

//   if( kMatrixSerpentineLayout == true) {
//     if (kMatrixVertical == false) {
//       if( y & 0x01) {
//         // Odd rows run backwards
//         uint8_t reverseX = (kMatrixWidth - 1) - x;
//         i = (y * kMatrixWidth) + reverseX;
//       } else {
//         // Even rows run forwards
//         i = (y * kMatrixWidth) + x;
//       }
//     } else { // vertical positioning
//       if ( x & 0x01) {
//         i = kMatrixHeight * (kMatrixWidth - (x+1))+y;
//       } else {
//         i = kMatrixHeight * (kMatrixWidth - x) - (y+1);
//       }
//     }
//   }
  
//   return i;
// }

// #define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
// CRGB* const leds( leds_plus_safety_pixel + 1);

// uint16_t XYsafe( uint8_t x, uint8_t y)
// {
//   if( x >= kMatrixWidth) return -1;
//   if( y >= kMatrixHeight) return -1;
//   return XY(x,y);
// }


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
