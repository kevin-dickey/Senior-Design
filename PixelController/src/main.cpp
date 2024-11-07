#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "../include/json.hpp"
#include "../lib/configuration/Configuration.h"

using json = nlohmann::json;

#define LED_PIN 13
#define SWITCH_BUF_PIN 2
#define NUM_LEDS_X 5
#define NUM_LEDS_Y 1
#define NUM_LED_CHANNELS 3
#define BUF_LEN_LEDS NUM_LEDS_X * NUM_LEDS_Y * NUM_LED_CHANNELS
#define MAX_BRIGHTNESS 64 // maximum for FastLED is 255, (don't go higher than like 8 if you don't have a PSU attached)

#if USE_EMULATOR

#define PROJECT_DIR SOURCE_ROOT

#else
#define PROJECT_DIR PROJECT_DIR

#define COLOR_ORDER GRB
#define CHIPSET WS2812B

#include <SPI_Impl.h>
#include <FastLED.h>

#include "../lib/configuration/Sensor.h"
#include "../lib/patternGeneration/rippleEffect.h"
#include "../lib/patternGeneration/utils.h"

#endif

enum ShiftDirection
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

SensorManager *sensorManager;
ESPSPI *espSPI;

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

// Array of the LEDs. Should be accessed using the XY functions (translation to 2D array, which is not done directly b/c
//                                                               of different possible layouts of the LEDs (serpentine n such))
CRGB leds[NUM_LEDS_X * NUM_LEDS_Y];
uint8_t out_buf[BUF_LEN_LEDS];
uint8_t in_buf[BUF_LEN_LEDS];

void setup()
{
    Serial.begin(115200);                    // for setting up stuff to print to serial monitor
    std::cout << "Starting..." << std::endl; // print to the serial monitor that the program is starting

    // Set up the SPI bus
    espSPI = new ESPSPI(true);
    espSPI->begin();

    pinMode(SWITCH_BUF_PIN, INPUT_PULLDOWN);

    std::cout << "Setup Complete..." << std::endl;

    delay(2000); // delay for 3 seconds to give time to open the serial monitor
}

/**
 * MARK: Looping
 */
void loop()
{
    // Make random output buffer
    for (size_t i = 0; i < BUF_LEN_LEDS; ++i)
    {
        out_buf[i] = i % 256;
    }

#ifdef SPI_HAS_TRANSACTION
    std::cout << "Using new SPI library syntax" << std::endl;

    long start = millis();
    // Number of attempts needed to wait until pico was ready to receive data
    uint8_t unsuccessful_attempts = 0;

    // Print the data to send to the pico
    std::cout << "Data to send: ";
    for (size_t i = 0; i < BUF_LEN_LEDS; ++i)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(out_buf[i]) << " ";
        if (i % 16 == 15)
        {
            std::cout << std::endl;
        }
        else if (i % 3 == 2)
        {
            std::cout << " ";
        }
    }
    std::cout << std::endl;

    for (uint8_t delayMs = 0; delayMs < 10; delayMs++)
    {
        espSPI->sendFrameData(out_buf, in_buf, BUF_LEN_LEDS);


        // Print the data received from the pico
        std::cout << "Data received: ";
        for (size_t i = 0; i < BUF_LEN_LEDS; ++i)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(in_buf[i]) << " ";
            if (i % 16 == 15)
            {
                std::cout << std::endl;
            }
            else if (i % 3 == 2)
            {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
#else
    // Error cause this isn't supported
    std::cout << "Using old SPI library syntax" << std::endl;
    std::cout << "Please use the new SPI library syntax" << std::endl;
    exit(1);
#endif
        std::cout << "Looping..." << std::endl;
        delay(1000);
    }
    while (true)
    {
        // Do nothing
    }
}

#endif
