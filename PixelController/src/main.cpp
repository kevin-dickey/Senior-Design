#include <iostream>
#include <fstream>

#include "../include/json.hpp"
#include "../lib/configuration/Configuration.h"
#include "../lib/configuration/Sensor.h"

using json = nlohmann::json;

#if USE_EMULATOR

#define PROJECT_DIR SOURCE_ROOT

#else
#define PROJECT_DIR PROJECT_DIR

#include <FastLED.h>
#define COLOR_ORDER GRB
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
Sensor *sensor0, *sensor2, *sensor3, *sensor12, *sensor14;
int hue;
char set_sensors;

std::vector<Sensor *> a_sensors;
std::vector<Sensor *> b_sensors;

void setup()
{
  Serial.begin(115200);          // for setting up stuff to print to serial monitor
  delay(3000);                   // delay for 3 seconds to give time to open the serial monitor
  Serial.println("Starting..."); // print to the serial monitor that the program is starting

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050); // setup the LEDs & LED pin for the esp32
  FastLED.setBrightness(MAX_BRIGHTNESS);                                                        // set the max brightness for the LEDs
  pinMode(LED_BUILTIN, OUTPUT);                                                                 // setup the built-in LED for the esp32
  Serial.println("Initialized FastLED...");

  Pair_t sensor_pos = {x : 0, y : 0};
  // TODO: Figure out the 0-indexing. Need to have same behavior on both sides
  sensor0 = new Sensor(0, 0, S_BINARY, sensor_pos);
  sensor2 = new Sensor(1, 4, S_BINARY, sensor_pos);
  sensor3 = new Sensor(2, 16, S_BINARY, sensor_pos);
  sensor12 = new Sensor(0, 12, S_BINARY, sensor_pos);
  sensor14 = new Sensor(1, 14, S_BINARY, sensor_pos);

  a_sensors = std::vector<Sensor *>{sensor0, sensor2, sensor3};
  b_sensors = std::vector<Sensor *>{sensor12, sensor14, sensor3};

  sensorManager = new SensorManager();
  sensorManager->setSensors(a_sensors);
  set_sensors = 'a';
  Serial.println("Initialized Sensors...");

  hue = 30;
}

/* Function Prototypes */

/**
 * MARK: Looping
 */
void loop()
{
  auto sensor_states = sensorManager->getSensorStates(true);

  if (Serial.read() == 'r' || sensor_states[2])
  {
    sensorManager->setSensors(set_sensors == 'a' ? b_sensors : a_sensors);
    set_sensors = set_sensors == 'a' ? 'b' : 'a';
  }

  if (sensor_states[0])
  {
    fill_solid(leds, NUM_LEDS, CRGB::DarkViolet);
  }
  else if (sensor_states[1])
  {
    fill_solid(leds, NUM_LEDS, CRGB::DarkOrange);
  }
  else
  {
    fill_rainbow(leds, NUM_LEDS, hue, 12); // fill the LEDs with a rainbow effect
    hue += 5;                              // increment the hue for the next frame
    hue %= 256;                            // keep the hue within the range of 0-255
  }

  FastLED.show(); // show the LEDs

  delay(1000);     // delay for 100 milliseconds
}

#endif
