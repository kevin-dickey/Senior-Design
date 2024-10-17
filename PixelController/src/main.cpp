#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include <SPI.h>

#include "../include/json.hpp"
#include "../lib/configuration/Configuration.h"

using json = nlohmann::json;

#define LED_PIN 13
#define NUM_LEDS_X 16
#define NUM_LEDS_Y 16
#define NUM_LEDS 256
#define MAX_BRIGHTNESS 64 // maximum for FastLED is 255, (don't go higher than like 8 if you don't have a PSU attached)

#if USE_EMULATOR

#define PROJECT_DIR SOURCE_ROOT

#else
#define PROJECT_DIR PROJECT_DIR

#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCK 18
#define VSPI_SS 5
#define BUF_LEN 0x100

#define COLOR_ORDER GRB
#define CHIPSET WS2812B

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
SPIClass *vspi = NULL;
CRGB leds[NUM_LEDS];
uint8_t out_buf[BUF_LEN], in_buf[BUF_LEN];

const uint32_t SPI_SPEED_HZ = 1000000; // 1 MHz

void setup()
{
    Serial.begin(115200);                    // for setting up stuff to print to serial monitor
    delay(3000);                             // delay for 3 seconds to give time to open the serial monitor
    std::cout << "Starting..." << std::endl; // print to the serial monitor that the program is starting

    Serial.print("MOSI Pin: ");
    Serial.println(VSPI_MOSI);
    Serial.print("MISO Pin: ");
    Serial.println(VSPI_MISO);
    Serial.print("SCK Pin: ");
    Serial.println(VSPI_SCK);

    vspi = new SPIClass(VSPI);
    vspi->begin(VSPI_SCK, VSPI_MISO, VSPI_MOSI, VSPI_SS);

    pinMode(VSPI_SS, OUTPUT); // VSPI SS

    std::cout << "Setup Complete..." << std::endl;

    // Initialize output buffer
    for (size_t i = 0; i < BUF_LEN; ++i)
    {
        // bit-inverted from i. The values should be: {0xff, 0xfe, 0xfd...}
        out_buf[i] = i;
    }
}

/**
 * MARK: Looping
 */
void loop()
{
#ifdef SPI_HAS_TRANSACTION
    std::cout << "Using new SPI library syntax" << std::endl;

    // Print the data to send
    std::cout << "Data to send:" << std::endl;
    for (size_t i = 0; i < BUF_LEN; ++i)
    {
        std::cout << static_cast<int16_t>(out_buf[i]) << " ";
        if ((i + 1) % 16 == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;

    vspi->beginTransaction(SPISettings(SPI_SPEED_HZ, MSBFIRST, SPI_MODE0));
    // Send data
    for (size_t i = 0; i < BUF_LEN; ++i)
    {
        in_buf[i] = vspi->transfer(out_buf[i]);
    }
    vspi->endTransaction();

    // Print the data received
    std::cout << "Data received:" << std::endl;
    for (size_t i = 0; i < BUF_LEN; ++i)
    {
        std::cout << static_cast<int16_t>(in_buf[i]) << " ";
        if ((i + 1) % 16 == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;

#else
#endif

    std::cout << "Looping..." << std::endl;
    delay(3000);
}

#endif
