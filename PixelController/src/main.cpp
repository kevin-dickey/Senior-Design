#define USE_EMULATOR 0

#include <fstream>
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

void rippleEffect(int r, int g, int b, uint8_t center_x, uint8_t center_y, int rippleCounter, int prevLeds[], int width);
uint8_t calculateDistance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
uint8_t scaleBrightness(uint8_t distance, uint8_t rippleCounter);  // depricated function
uint16_t XY(uint8_t x, uint8_t y);
uint16_t XYsafe(uint8_t x, uint8_t y);
void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8);  // draws rainbow frame

void parseBitmapData(const char *hexData);
void parseBitmapData(const uint32_t *hexData, size_t length);
CRGB hexToCRGB(const char *hex);
CRGB hexToCRGB(uint32_t hex);
void loadBMP(const char *filename);

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

uint32_t ghost[NUM_LEDS] = {
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x0000FF, 0x000000, 0x000000,
    0x000000, 0x0000FF, 0x0000FF, 0x00FFFF, 0x00FFFF, 0x0000FF, 0x0000FF, 0x000000,
    0x000000, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x000000,
    0x000000, 0x00FFFF, 0x00FFFF, 0x0000FF, 0x0000FF, 0x00FFFF, 0x00FFFF, 0x000000,
    0x000000, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x000000,
    0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};

void setup() {
  Serial.begin(115200);                                                                          // for setting up stuff to print to serial monitor
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);  // setup the LEDs & LED pin for the esp32
  FastLED.setBrightness(MAX_BRIGHTNESS);                                                         // set the max brightness for the LEDs
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  // Parse the bitmap data
  // parseBitmapData(pumpkin);          // works
  // parseBitmapData(ghost, NUM_LEDS);  // doesn't work
  loadBMP("../ghost.bmp");  // unable to find the image for some reason
}

void loop() {
  FastLED.show();
  delay(1000);  // Adjust the delay as needed
}

void loadBMP(const char *filename) {
  std::ifstream file(filename, std::ios::binary);

  if (!file) {
    Serial.printf("Couldn't find file");
    return;
  }
  Serial.printf("File has been found");

  // Skip the BMP header
  file.seekg(18);  // Width offset
  int width, height;
  file.read(reinterpret_cast<char *>(&width), 4);
  file.read(reinterpret_cast<char *>(&height), 4);

  if (width != NUM_LEDS_X || height != NUM_LEDS_Y) {
    std::cerr << "Unsupported image dimensions. Only 16x16 images are supported." << std::endl;
    return;
  }

  file.seekg(28);  // Bits per pixel offset
  short bpp;
  file.read(reinterpret_cast<char *>(&bpp), 2);

  if (bpp != 24) {
    std::cerr << "Unsupported color depth. Only 24-bit BMP images are supported." << std::endl;
    return;
  }

  // Calculate the offset to the pixel data
  file.seekg(10);
  int dataOffset;
  file.read(reinterpret_cast<char *>(&dataOffset), 4);

  file.seekg(dataOffset);

  // Read pixel data
  for (int y = NUM_LEDS_Y - 1; y >= 0; --y) {
    for (int x = 0; x < NUM_LEDS_X; ++x) {
      char blue, green, red;
      file.read(&blue, 1);
      file.read(&green, 1);
      file.read(&red, 1);

      int index = (y * NUM_LEDS_X) + x;
      leds[index] = CRGB(red, green, blue);
    }

    // Skip padding bytes (BMP rows are padded to 4 bytes)
    file.seekg((4 - (NUM_LEDS_X * 3) % 4) % 4, std::ios::cur);
  }
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

// Function to convert a hexadecimal color to CRGB
CRGB hexToCRGB(uint32_t hex) {
  uint8_t r = (hex >> 16) & 0xFF;
  uint8_t g = (hex >> 8) & 0xFF;
  uint8_t b = hex & 0xFF;
  return CRGB(r, g, b);
}

// Function to parse the bitmap data from a hexadecimal string
void parseBitmapData(const uint32_t *hexData, size_t length) {
  for (size_t i = 0; i < length && i < NUM_LEDS; i++) {
    leds[i] = hexToCRGB(hexData[i]);
  }
}

#endif
