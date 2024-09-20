#include <iostream>

#include <fstream>
#include "../include/json.hpp"
#include "../lib/configuration/Configuration.h"

using json = nlohmann::json;

#if USE_EMULATOR

#define PROJECT_DIR SOURCE_ROOT

#else
#define PROJECT_DIR PROJECT_DIR

#include <FastLED.h>
#define COLOR_ORDER RBG
#define CHIPSET WS2811

#endif

SensorManager *sensorManager;

#if USE_EMULATOR

int main()
{
  sensorManager = new SensorManager();

  std::string filePath = std::string(PROJECT_DIR) + "/lib/configuration/test/Basic_Show_File.json";
  Show show = loadShow(filePath);
  std::cout << "test";

  auto *gridLayout = dynamic_cast<GridLayout *>(show.layouts[0]);
  std::cout << "Grid Layout Width: " << gridLayout->width << std::endl;
  std::cout << "Grid Layout Height: " << gridLayout->height << std::endl;

  return 0;
}

#else

#define LED_PIN 13
#define NUM_LEDS_X 16
#define NUM_LEDS_Y 16
#define NUM_LEDS 60 
#define MAX_BRIGHTNESS 25 // maximum for FastLED is 255, but I would probably not go higher than 64 (ESPECIALLY if no power supply)

/**
 * MARK: Setup
 */
// Array of the LEDs. Should be accessed using the XY functions (translation to 2D array, which is not done directly b/c
//   of different possible layouts of the LEDs (serpentine n such))
CRGB leds[NUM_LEDS];
Sensor *sensor0, *sensor2, *sensor12, *sensor14;
int hue;


void setup()
{
  Serial.begin(115200);                                                                           // for setting up stuff to print to serial monitor

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050); // setup the LEDs & LED pin for the esp32
  FastLED.setBrightness(MAX_BRIGHTNESS);                                                        // set the max brightness for the LEDs
  pinMode(LED_BUILTIN, OUTPUT); // setup the built-in LED for the esp32

  sensor0 = new Sensor(0, 0, BINARY, new Pair(0, 0));
  sensor2 = new Sensor(0, 2, BINARY, new Pair(0, 0));
  sensor12 = new Sensor(1, 12, BINARY, new Pair(0, 0));
  sensor14 = new Sensor(2, 14, BINARY, new Pair(0, 0));

  a_sensors = std::vector<Sensor*>{sensor0, sensor2};
  b_sensors = std::vector<Sensor*>{sensor12, sensor14};

  sensorManager = new SensorManager();
  sensorManager->addSensors(a_sensors);

  hue = 30;
}

/* Function Prototypes */

/**
 * MARK: Looping
 */
void loop()
{
  if (digitalRead(SENSOR_PIN) == LOW){
    std::cout << "Sensor triggered" << std::endl;
    fill_solid(leds, NUM_LEDS, CRGB::CadetBlue);
    FastLED.show();
    delay(2000);
    return;
  }
  fill_rainbow(leds, NUM_LEDS, hue, 7); // fill the LEDs with a rainbow effect
  hue += 1;                             // increment the hue for the next frame
  hue %= 256;                           // keep the hue within the range of 0-255
  FastLED.show();                       // show the LEDs
  delay(1000 / 60);                     // delay for 60fps
}

#endif
