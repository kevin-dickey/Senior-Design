#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "../include/json.hpp"
#include "../lib/configuration/Configuration.h"

using json = nlohmann::json;

#define LED_PIN 13
#define NUM_LEDS_X 16
#define NUM_LEDS_Y 16
#define NUM_LEDS 256
#define MAX_BRIGHTNESS 4 // maximum for FastLED is 255, but I would probably not go higher than 64 (ESPECIALLY if no power supply)

#if USE_EMULATOR

#define PROJECT_DIR SOURCE_ROOT

#else
#define PROJECT_DIR PROJECT_DIR

#define COLOR_ORDER GRB
#define CHIPSET WS2812B

#include <FastLED.h>
#include "../lib/configuration/Sensor.h"
#include "../lib/patternGeneration/utils.h"
#include "../lib/patternGeneration/rippleEffect.h"

#endif

enum ShiftDirection
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

SensorManager *sensorManager;

#if USE_EMULATOR

int main()
{
    sensorManager = new SensorManager();

    std::string filePath = std::string(PROJECT_DIR) + "/lib/configuration/test/Basic_Show_File.json";
    Show show = loadShow(filePath);

    auto *gridLayout = dynamic_cast<GridLayout *>(show.layouts[0]);
    std::cout << "Grid Layout Width: " << gridLayout->width << std::endl;
    std::cout << "Grid Layout Height: " << gridLayout->height << std::endl;

    return 0;
}

#else

/**
 * MARK: Setup
 */

void fadeToBlack(int duration);
void fadeToBrightness(int duration, int targetBrightness);

void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8); // draws rainbow frame

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

// const unsigned char ghost8bit[] = {
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0x3f, 0x3f, 0x3f, 0x1f, 0x1f, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xe0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xe0, 0xe0, 0xe0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x07, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf0, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xf8, 0xf8, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

// const unsigned char pumptest[] = {
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf8, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf8, 0xfc, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

Sensor *sensor0, *sensor2, *sensor3, *sensor12, *sensor14;
int hue;
int count;
bool goUp;
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
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
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
        
        // Testing effect 4 for softlaunch
        loadHexBitmap(leds, ghost8bit, 8, 4, 8, 8); // startx = 8, starty = 4, bitmapheight = 8, bitmapwidth = 8
        loadHexBitmap(leds, pumpkin8bit, 0, 4, 8, 8);
}

/* Function Prototypes */

/**
 * MARK: Looping
 */
void loop()
{
    auto sensor_states = sensorManager->getSensorStates(true);

    /*
       adding the code for how the effects will need to be structured to work properly,
       needs integration to trigger given the correct sensor input

       (need to look at the sensor documentation later to figure out how to do that)

       NOTE: IN THE ACTUAL CODE, THE LOADING OF THE BITMAP SHOULD NOT BE REPEATEDLY CALLED!
    */

    // effect 1 (0-0), rainbow + still pumpkin
    // neither button triggered

        // rainbow
        // uint32_t ms = millis();
        // int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / kMatrixWidth));
        // int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / kMatrixHeight));
        // DrawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
        // if (ms < 5000) {
        //     FastLED.setBrightness(scale8(MAX_BRIGHTNESS, (ms * 256) / 5000));
        // } else {
        //     FastLED.setBrightness(MAX_BRIGHTNESS);
        // }
        // // draw pumpkin on top
        // loadHexBitmap(leds, pumpkin8bit, 4, 4, 8, 8);
        // FastLED.show(); // commented out b/c loadHexBitmap already calls it, but here for clarity

    // effect 2 (0-1), gravestone + ghost doing *something*
    // button 1 triggered

        // ┌ might be difficult to get to show up well on LEDs.
        // v
        // loadHexBitmap(leds, gravestone, ?, ?, ?, ?);
        // loadHexBitmap(leds, ghost8bit, ?, ?, 8, 8);
        // whatever we decide on

    // effect 3 (1-0), ghost zig zagging
    // button 2 triggered

        // loadHexBitmap(leds, ghost8bit, 4, 4, 8, 8); // startx = 4, starty = 4, bitmapheight = 8, bitmapwidth = 8
        // shiftLeds(leds, RIGHT);
        // if (count % 2 == 0) {
        //     if (goUp) {
        //         shiftLeds(leds, UP);
        //     } else {
        //         shiftLeds(leds, DOWN);
        //     }
        // }
        // count++;
        // // Reverse the vertical direction every 4 frames (2 up, 2 down)
        // if (count % 4 == 0) {
        //     goUp = !goUp;
        // }

    // effect 4 (1-1), pumpkin + ghost chasing each other
    // button 1 and 2 triggered

        // loadHexBitmap(leds, ghost8bit, 8, 4, 8, 8); // startx = 8, starty = 4, bitmapheight = 8, bitmapwidth = 8
        // loadHexBitmap(leds, pumpkin8bit, 0, 4, 8, 8);
        shiftLeds(leds, RIGHT);
        if (count % 2 == 0) {
            if (goUp) {
                shiftLeds(leds, UP);
            } else {
                shiftLeds(leds, DOWN);
            }
        }
        count++;
        // Reverse the vertical direction every 4 frames (2 up, 2 down)
        if (count % 4 == 0) {
            goUp = !goUp;
        } 
        // below can look a little gross when shifts too far up/down
        // else if (count % ((NUM_LEDS_X + NUM_LEDS_Y) / 2) == 0) { // introduce some randomness every full shift
        //     int random = rand() % 4 + 1;
        //     switch (random) {
        //         case 1:
        //             shiftLeds(leds, RIGHT);
        //             break;
        //         case 2:
        //             shiftLeds(leds, LEFT);
        //             break;
        //         case 3:
        //             shiftLeds(leds, UP);
        //             break;
        //         case 4:
        //             shiftLeds(leds, DOWN);
        //             break;
        //         default:
        //             Serial.println("Error generating random number for shifting");
        //             break;
        //     }
        // }


    delay(33); // delay(33): approx 30fps (30.3)
}

// Function to load an 8x8 bitmap from a hex string
void loadHexBitmap(CRGB *leds, const char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth)
{
    for (uint8_t y = 0; y < bitmapHeight; y++)
    {
        for (uint8_t x = 0; x < bitmapWidth; x++)
        {
            // Calculate the position in the string
            int index = (y * bitmapWidth + x) * 7; // 6 for color + 1 for space

            // Extract the hex color (6 characters)
            char hexColor[7]; // 6 for color + 1 for null terminator
            strncpy(hexColor, &bitmap[index], 6);
            hexColor[6] = '\0'; // Null-terminate the string

            // Only load if within bounds
            if (startX + x < kMatrixWidth && startY + y < kMatrixHeight)
            {
                CRGB color = hexToCRGB(hexColor);
                if (color != (CRGB::Black))
                { // black color is interpreted as intending to be transparent
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
void loadByteBitmap(CRGB *leds, const unsigned char *bitmap, uint8_t startX, uint8_t startY, int bitmapHeight, int bitmapWidth)
{
    for (uint8_t y = 0; y < bitmapHeight; y++)
    {
        for (uint8_t x = 0; x < bitmapWidth; x++)
        {
            // Only draw if within bounds of the LED matrix
            if ((startX + x) < kMatrixWidth && (startY + y) < kMatrixHeight)
            {
                // Calculate the index for this pixel (3 bytes per pixel: RGB)
                int index = (y * bitmapWidth + x) * 3;

                // Extract RGB values from the hex array
                char hexColor[7];
                snprintf(hexColor, sizeof(hexColor), "%02x%02x%02x", bitmap[index], bitmap[index + 1], bitmap[index + 2]);
                hexColor[6] = '\0';

                if (startX + x < kMatrixHeight && startY + y < kMatrixHeight)
                {
                    CRGB color = hexToCRGB(hexColor);
                    if (color != (CRGB::Black))
                    {
                        leds[XY(startX + x, startY + y)] = color;
                    }
                }
            }
        }
    }
    FastLED.show(); // Display the updated LED matrix
}

void shiftLeds(CRGB leds[], ShiftDirection direction)
{
    CRGB temp[NUM_LEDS_X * NUM_LEDS_Y];

    // Copy current state to temp array
    for (int i = 0; i < NUM_LEDS_X * NUM_LEDS_Y; i++)
    {
        temp[i] = leds[i];
    }

    switch (direction)
    {
    case RIGHT: // if you look closely right and left might look flipped, and you're right!
                // don't ask me why, it just works :)
        for (int y = 0; y < NUM_LEDS_Y; y++)
        {
            for (int x = 0; x < NUM_LEDS_X; x++)
            {
                int newX = (x - 1 + NUM_LEDS_X) % NUM_LEDS_X;
                leds[XY(newX, y)] = temp[XY(x, y)];
            }
        }
        break;

    case LEFT:
        for (int y = 0; y < NUM_LEDS_Y; y++)
        {
            for (int x = 0; x < NUM_LEDS_X; x++)
            {
                int newX = (x + 1) % NUM_LEDS_X;
                leds[XY(newX, y)] = temp[XY(x, y)];
            }
        }
        break;

    case UP:
        for (int x = 0; x < NUM_LEDS_X; x++)
        {
            for (int y = 0; y < NUM_LEDS_Y; y++)
            {
                int newY = (y - 1 + NUM_LEDS_Y) % NUM_LEDS_Y;
                leds[XY(x, newY)] = temp[XY(x, y)];
            }
        }
        break;

    case DOWN:
        for (int x = 0; x < NUM_LEDS_X; x++)
        {
            for (int y = 0; y < NUM_LEDS_Y; y++)
            {
                int newY = (y + 1) % NUM_LEDS_Y;
                leds[XY(x, newY)] = temp[XY(x, y)];
            }
        }
        break;
    }

    FastLED.show(); // Update the LED display
}

/**
 * duration is given in seconds.
 *
 * Uses FastLED's builtin for setting brightness, so it'll modify every LED.
 *
 * If you want to use it on a specific subset of LEDs, will have to provide
 * that subset as well as somehow keeping track of what the LEDs previously were.
 */
void fadeToBlack(int duration)
{
    uint8_t initialBrightness = FastLED.getBrightness();
    if (initialBrightness == 0)
        return;

    int updatesPerSec = initialBrightness / duration;

    for (int i = initialBrightness; i > 0; i--)
    {
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
void fadeToBrightness(int duration, int targetBrightness)
{
    uint8_t curBrightness = FastLED.getBrightness();
    if (curBrightness == targetBrightness)
        return;

    int updatesPerSec;
    if (targetBrightness > curBrightness)
    { // increase brightness to target
        updatesPerSec = (targetBrightness - curBrightness) / duration;
        for (int i = curBrightness; i < targetBrightness; i++)
        {
            FastLED.setBrightness(i);
            FastLED.show();
            delay(1000 / updatesPerSec);
        }
    }
    else
    { // decrease brightness to target
        updatesPerSec = (curBrightness - targetBrightness) / duration;
        for (int i = curBrightness; i > targetBrightness; i--)
        {
            FastLED.setBrightness(i);
            FastLED.show();
            delay(1000 / updatesPerSec);
        }
    }

    FastLED.setBrightness(targetBrightness); // just in case it doesnt fully work lol
    FastLED.show();
}

// Function to convert a 6-character hex string to CRGB
CRGB hexToCRGB(const char *hex)
{
    uint8_t r = strtol(std::string(hex, 2).c_str(), NULL, 16);
    uint8_t g = strtol(std::string(hex + 2, 2).c_str(), NULL, 16);
    uint8_t b = strtol(std::string(hex + 4, 2).c_str(), NULL, 16);
    return CRGB(r, g, b);
}

// Function to parse the bitmap data from a hex string
void parseBitmapData(const char *hexData)
{
    int index = 0;
    while (*hexData)
    {
        // Skip spaces
        if (*hexData == ' ')
        {
            hexData++;
            continue;
        }

        // Convert the next 6 characters to CRGB and store in the leds array
        if (index < NUM_LEDS)
        {
            leds[index] = hexToCRGB(hexData);
            hexData += 6; // Move to the next color
            index++;
        }
        else
        {
            break; // Avoid exceeding the array size
        }
    }
}

/**
 * Draws a single frame of the rainbow effect
 */
void DrawOneFrame(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
    uint8_t lineStartHue = startHue8;
    for (uint8_t y = 0; y < kMatrixHeight; y++)
    {
        lineStartHue += yHueDelta8;
        uint8_t pixelHue = lineStartHue;
        for (uint8_t x = 0; x < kMatrixWidth; x++)
        {
            pixelHue += xHueDelta8;
            leds[XY(x, y)] = CHSV(pixelHue, 255, 255);
        }
    }
}
#endif
