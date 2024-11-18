#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <chrono>
#include "../include/json.hpp"
#include "../lib/configuration/Configuration.h"
#include "../lib/configuration/ControllerRunner.h"

using json = nlohmann::json;

#define LED_PIN 13
#define NUM_LEDS_X 16
#define NUM_LEDS_Y 16
#define NUM_LEDS 256
#define MAX_BRIGHTNESS 64  // maximum for FastLED is 255, (don't go higher than like 8 if you don't have a PSU attached)

#if USE_EMULATOR

#define PROJECT_DIR SOURCE_ROOT

#else
#define PROJECT_DIR PROJECT_DIR

#define COLOR_ORDER GRB
#define CHIPSET WS2812B

#include <FastLED.h>

#include "../lib/configuration/Sensor.h"
#include "../lib/configuration/ControllerRunner.h"
#include "../lib/patternGeneration/rippleEffect.h"
#include "../lib/patternGeneration/utils.h"

#endif

enum ShiftDirection {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

SensorManager *sensorManager;
ControllerRunner* runner;
unsigned long showStart = 0;
std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long, std::ratio<1,1000000000>>>epoch;

unsigned long getMillis(){
#if USE_EMULATOR
    auto now = std::chrono::high_resolution_clock ::now();
    auto mseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - epoch).count();
    return mseconds;
#else

    return millis();
#endif
}

#if USE_EMULATOR

int main() {

    epoch = std::chrono::high_resolution_clock::from_time_t(0);
    sensorManager = new SensorManager();

    std::string filePath = std::string(PROJECT_DIR) + "/lib/configuration/test/Basic_Show_File.json";
    Show show = loadShow(filePath);

//    auto *gridLayout = dynamic_cast<GridLayout *>(show.layouts[0]);
//    std::cout << "Grid Layout Width: " << gridLayout->width << std::endl;
//    std::cout << "Grid Layout Height: " << gridLayout->height << std::endl;

    showStart = getMillis();
    runner = new ControllerRunner(show, showStart, epoch);
    auto sensor_state = std::vector<bool>{false, false, false, false};

    for(int i = 0 ; i < 30 ; i++) {
        if (i == 11) {
            sensor_state = std::vector<bool>{false, false, false, false};
        }
        if (i == 10) {
            // sensorValues = sensorManager->getSensors()
            sensor_state = std::vector<bool>{true, false, false, false};
        }
        if((show.duration - runner->getTotalSensorRuntime() <= runner->getEffectCursor())){
            showStart = getMillis();
            epoch = std::chrono::high_resolution_clock::from_time_t(0);
            runner = new ControllerRunner(show, showStart, epoch);
        }

        auto showFrame = runner->getNextShowFrame(sensor_state);
        if(showFrame.effect->name!= "no effect" && showFrame.frame != -1)
        {
            std::cout << "Effect: " << showFrame.effect->name << std::endl;
            std::cout << "Frame: " << showFrame.frame << std::endl;
        }
        else
        {
            std::cout << "no effect found. resetting runner " << std::endl;
            showStart = getMillis();
            epoch = std::chrono::high_resolution_clock::from_time_t(0);
            runner = new ControllerRunner(show, showStart, epoch);
        }
    }
    return 0;
}

#else

/**
 * MARK: Setup
 */

void fadeToBlack(int duration);
void fadeToBrightness(int duration, int targetBrightness);

void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8);  // draws rainbow frame
void DrawOneFrameReducedBright(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8); // ^ @ half brightness

void parseBitmapData(const char *hexData);
void loadHexBitmap(CRGB *leds, const char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth);
void loadByteBitmap(CRGB *leds, const unsigned char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth);
CRGB hexToCRGB(const char *hex);

void shiftLeds(CRGB leds[], ShiftDirection direction);

// Array of the LEDs. Should be accessed using the XY functions (translation to 2D array, which is not done directly b/c
//                                                               of different possible layouts of the LEDs (serpentine n such))
CRGB leds[NUM_LEDS];
int prevLeds1[NUM_LEDS] = {0};
int prevLeds2[NUM_LEDS] = {0};
int prevLeds3[NUM_LEDS] = {0};
int prevLeds4[NUM_LEDS] = {0};

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

const char *pumpkin8bit =
    "000000 000000 000000 000000 267f00 267f00 000000 000000 "
    "000000 000000 000000 267f00 267f00 000000 000000 000000 "
    "000000 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 000000 "
    "ff6100 ff6100 ffec1e ff6100 ff6100 ffec1e ff6100 ff6100 "
    "ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 "
    "ff6100 ffec1e ff6100 ff6100 ff6100 ff6100 ffec1e ff6100 "
    "ff6100 ff6100 ffec1e ffec1e ffec1e ffec1e ff6100 ff6100 "
    "000000 ff6100 ff6100 ff6100 ff6100 ff6100 ff6100 000000";

const char *ghost8bit = "000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff 000000 ffffff ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff ffffff 000000 ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff ffffff 000000 000000 ffffff ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff 000000 ffffff ffffff 000000 ffffff 000000";

const char *skull8bit = "000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000 ffffff ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff ffffff 000000 000000 000000 000000 000000 000000 000000 ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff 000000 000000 000000 000000 000000 000000 000000 000000 ffffff 000000 ffffff 000000 000000 ffffff 000000 ffffff 000000 000000 000000 000000 000000 000000 000000 000000 ffffff ffffff ffffff ffffff ffffff ffffff ffffff ffffff 000000 000000 000000 000000";

                        // const unsigned char ghost8bit[] = {
                        //     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0x3f, 0x3f, 0x3f, 0x1f, 0x1f, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xe0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xe0, 0xe0, 0xe0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x07, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf0, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xf8, 0xf8, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

                        // const unsigned char pumptest[] = {
                        //     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf8, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf8, 0xfc, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

                        Sensor *
                        sensor0,
           *sensor1, *sensor2, *sensor3;
int hue;
int count;
bool goUp;
char set_sensors;

std::vector<Sensor *> a_sensors;
std::vector<bool> prev_sensor_triggered;


void setup() {
    Serial.begin(115200);           // for setting up stuff to print to serial monitor
    delay(3000);                    // delay for 3 seconds to give time to open the serial monitor
    Serial.println("Starting...");  // print to the serial monitor that the program is starting

    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);  // setup the LEDs & LED pin for the esp32
    FastLED.setBrightness(MAX_BRIGHTNESS);                                                         // set the max brightness for the LEDs
    pinMode(LED_BUILTIN, OUTPUT);                                                                  // setup the built-in LED for the esp32
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    Serial.println("Initialized FastLED...");

    Pair_t sensor_pos{0, 0};
    Pair_t size = {10, 10};
    Effect* sensorEffect = new Effect(1, "BasicEffect", sensor_pos, size, 0.0, 5000.0, nullptr);

    // TODO: Figure out the 0-indexing. Need to have same behavior on both sides
    sensor0 = new Sensor(0, 34, 1000, S_BINARY, sensor_pos, sensorEffect);  // ne
    sensor1 = new Sensor(1, 35, 1000, S_BINARY, sensor_pos, sensorEffect);  // nw
    sensor2 = new Sensor(2, 32, 1000, S_BINARY, sensor_pos, sensorEffect);  // sw
    sensor3 = new Sensor(3, 33, 1000, S_BINARY, sensor_pos, sensorEffect);  // se
    
    a_sensors = std::vector<Sensor *>{sensor0, sensor1, sensor2, sensor3};
    prev_sensor_triggered = std::vector<bool>(a_sensors.size());

    sensorManager = new SensorManager();
    sensorManager->setSensors(a_sensors);
    set_sensors = 'a';
    Serial.println("Initialized Sensors...");

    // Setup Runner
    //std::string filePath = std::string(PROJECT_DIR) + "/lib/configuration/test/Basic_Show_File.json";
    std::string filePath = "C:/Users/Eleen/Desktop/Senior Design/sddec24-15/sddec24-15/PixelController/data/show.json";
    Show show = loadShow(filePath);
    runner = new ControllerRunner(show, millis(), epoch);

    hue = 30;
    count = 0;
    goUp = true;
    srand(static_cast<unsigned int>(time(0)));

    /* Testing Stuff */
    // these don't work
    // loadByteBitmap(leds, pumptest, 4, 4, 8, 8);
    // loadByteBitmap(leds, ghost8bit, 4, 4, 8, 8);

    // if this doesn't work, something is seriously messed up
    // loadHexBitmap(leds, pumpkin8bit, 4, 4, 8, 8);

    // loadHexBitmap(leds, ghost8bit, 4, 4, 8, 8);
}

/**
 * Resets every index in prev_sensor_triggered to false except for the specified index, which is set to true.
 */
void resetTriggerMarkers(int exception) {
    for (int i = 0; i < prev_sensor_triggered.size(); i++) {
        if (i == exception) {
            prev_sensor_triggered[i] = true;
        } else {
            prev_sensor_triggered[i] = false;
        }
    }
}

/**
 * MARK: Looping
 */
void loop() {
    // gets the current state of every sensor, the state automatically resets after it's viewed
    auto sensor_states = sensorManager->getSensorStates(true);
    auto current_millis = millis();
   
    // sets the active sensors so that the runner is constanly checking sensor state and determining what to display
    // get showFrame
    auto showFrame = runner->getNextShowFrame(sensor_states);
    while(showFrame.effect->name== "no effect" && showFrame.frame == -1)
    {
            //no effect found because we're over the shows duration. reset show
            std::string filePath = "C:/Users/Eleen/Desktop/Senior Design/sddec24-15/sddec24-15/PixelController/data/show.json";
            Show show = loadShow(filePath);
            showStart = getMillis();
            epoch = std::chrono::high_resolution_clock::from_time_t(0);
            runner = new ControllerRunner(show, showStart, epoch);
            showFrame = runner->getNextShowFrame(sensor_states);
    }

    //ToDo
    // leaving this code here for reference. Needs to be removed
    // // check the states of sensors, set the markers accordingly for which codeblock to execute
    // if (effects includes rainbow + still pumpkin) {         // sensor0 -- rainbow + still pumpkin
    //     runRainbowAndSkull(frame)
    // } else if (effect = skull effect) {  // sensor1 -- skull
    //     runSkull(frame)
    //     for effect in effects:
    //         runEffect("skull", effect.frame)
    // } else if (effect = ghost effect) {  // sensor2 -- ghost zig zagging
    //     fill_solid(leds, NUM_LEDS, CRGB::Black);
    //     FastLED.setBrightness(MAX_BRIGHTNESS);
    //     FastLED.show();
    //     loadHexBitmap(leds, ghost8bit, 4, 4, 8, 8);  // startx = 4, starty = 4, bitmapheight = 8, bitmapwidth = 8
    //     Serial.println("Sensor 2 triggered");

    //     resetTriggerMarkers(2);
    //     count = 0;
    //     goUp = true;
    // } else if (effect = ghost and pumpkin effect) {  // sensor3 -- pumpkin & ghost chasing each other
    //     fill_solid(leds, NUM_LEDS, CRGB::Black);
    //     FastLED.setBrightness(MAX_BRIGHTNESS);
    //     FastLED.show();
    //     loadHexBitmap(leds, ghost8bit, 8, 4, 8, 8);  // startx = 8, starty = 4, bitmapheight = 8, bitmapwidth = 8
    //     loadHexBitmap(leds, pumpkin8bit, 0, 4, 8, 8);
    //     Serial.println("Sensor 3 triggered");

    //     resetTriggerMarkers(3);
    //     count = 0;
    //     goUp = true;
    // }

    // /************************************************************* */

    // // check to see what was the last effect triggered, keep running it
    // if (prev_sensor_triggered[0]) { // rainbow & pumpkin
    //     uint32_t ms = millis();
    //     int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / kMatrixWidth));
    //     int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / kMatrixHeight));
    //     DrawOneFrameReducedBright(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
        
    //     // draw pumpkin on top
    //     loadHexBitmap(leds, pumpkin8bit, 4, 4, 8, 8);
    //     // FastLED.show(); // commented out b/c loadHexBitmap already calls it, but here for clarity

    // } else if (prev_sensor_triggered[1]) { // skull/crossbones

    // /***** these are blocking! will have to figure out something else to go here *****/
    //     fadeToBrightness(2, MAX_BRIGHTNESS / 4);
    //     fadeToBrightness(2, MAX_BRIGHTNESS);

    // } else if (prev_sensor_triggered[2]) { // ghost zigzagging
    //     // shift right every frame, vertically every two frames, changes vertical direction every 4 frames
    //     shiftLeds(leds, RIGHT);
    //     if (count % 2 == 0) {
    //         shiftLeds(leds, goUp ? UP : DOWN);
    //     }
        
    //     if (count % 4 == 0) {
    //         goUp = !goUp;
    //     }
    //     count++;

    // } else if (prev_sensor_triggered[3]) { // pumpkin & ghost chasing each other
    //     // shift right every frame, vertically every two frames, changes vertical direction every 4 frames
    //     shiftLeds(leds, RIGHT);
    //     if (count % 2 == 0) {
    //         shiftLeds(leds, goUp ? UP : DOWN);
    //     }
        
    //     if (count % 4 == 0) {
    //         goUp = !goUp;
    //     }
    //     count++;
    // }

    //Kevins scoop:
    // run(showFrame); 

    delay(33);  // delay(33): approx 30fps (30.3)
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

/**
 * DOESN'T WORK. DUNNO WHY!
 */
void loadByteBitmap(CRGB *leds, const unsigned char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth) {
    for (uint8_t y = 0; y < bitmapHeight; y++) {
        for (uint8_t x = 0; x < bitmapWidth; x++) {
            // Only draw if within bounds of the LED matrix
            if ((startX + x) < kMatrixWidth && (startY + y) < kMatrixHeight) {
                // Calculate the index for this pixel (3 bytes per pixel: RGB)
                int index = (y * bitmapWidth + x) * 3;

                // Extract RGB values from the hex array
                char hexColor[7];
                snprintf(hexColor, sizeof(hexColor), "%02x%02x%02x", bitmap[index], bitmap[index + 1], bitmap[index + 2]);
                hexColor[6] = '\0';

                if (startX + x < kMatrixHeight && startY + y < kMatrixHeight) {
                    CRGB color = hexToCRGB(hexColor);
                    if (color != (CRGB::Black)) {
                        leds[XY(startX + x, startY + y)] = color;
                    }
                }
            }
        }
    }
    FastLED.show();  // Display the updated LED matrix
}

void shiftLeds(CRGB leds[], ShiftDirection direction) {
    CRGB temp[NUM_LEDS_X * NUM_LEDS_Y];

    // Copy current state to temp array
    for (int i = 0; i < NUM_LEDS_X * NUM_LEDS_Y; i++) {
        temp[i] = leds[i];
    }

    switch (direction) {
        case RIGHT:  // if you look closely right and left might look flipped, and you're right!
                     // don't ask me why, it just works :)
            for (int y = 0; y < NUM_LEDS_Y; y++) {
                for (int x = 0; x < NUM_LEDS_X; x++) {
                    int newX = (x - 1 + NUM_LEDS_X) % NUM_LEDS_X;
                    leds[XY(newX, y)] = temp[XY(x, y)];
                }
            }
            break;

        case LEFT:
            for (int y = 0; y < NUM_LEDS_Y; y++) {
                for (int x = 0; x < NUM_LEDS_X; x++) {
                    int newX = (x + 1) % NUM_LEDS_X;
                    leds[XY(newX, y)] = temp[XY(x, y)];
                }
            }
            break;

        case UP:
            for (int x = 0; x < NUM_LEDS_X; x++) {
                for (int y = 0; y < NUM_LEDS_Y; y++) {
                    int newY = (y - 1 + NUM_LEDS_Y) % NUM_LEDS_Y;
                    leds[XY(x, newY)] = temp[XY(x, y)];
                }
            }
            break;

        case DOWN:
            for (int x = 0; x < NUM_LEDS_X; x++) {
                for (int y = 0; y < NUM_LEDS_Y; y++) {
                    int newY = (y + 1) % NUM_LEDS_Y;
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

/**
 * Draws a single frame of the rainbow effect
 */
void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
    uint8_t lineStartHue = startHue8;
    for (uint8_t y = 0; y < kMatrixHeight; y++) {
        lineStartHue += yHueDelta8;
        uint8_t pixelHue = lineStartHue;
        for (uint8_t x = 0; x < kMatrixWidth; x++) {
            pixelHue += xHueDelta8;
            leds[XY(x, y)] = CHSV(pixelHue, 255, 255);
        }
    }
}

/**
 * Draws a single frame of the rainbow effect
 */
void DrawOneFrameReducedBright(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
    uint8_t lineStartHue = startHue8;
    for (uint8_t y = 0; y < kMatrixHeight; y++) {
        lineStartHue += yHueDelta8;
        uint8_t pixelHue = lineStartHue;
        for (uint8_t x = 0; x < kMatrixWidth; x++) {
            pixelHue += xHueDelta8;
            leds[XY(x, y)] = CHSV(pixelHue * 5 / 6, 255 * 5 / 6, 255 * 5 / 6);
        }
    }
}

#endif
