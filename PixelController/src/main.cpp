#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include <SPI.h>

#include "../include/json.hpp"
#include "../lib/configuration/Configuration.h"

using json = nlohmann::json;

#define LED_PIN 13
#define NUM_LEDS_X 60
#define NUM_LEDS_Y 40
#define MAX_BRIGHTNESS 64 // maximum for FastLED is 255, (don't go higher than like 8 if you don't have a PSU attached)

#if USE_EMULATOR

#define PROJECT_DIR SOURCE_ROOT

#else
#define PROJECT_DIR PROJECT_DIR

#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCK 18
#define VSPI_SS 5

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
CRGB leds[NUM_LEDS_X * NUM_LEDS_Y];
uint8_t out_buf[NUM_LEDS_X * NUM_LEDS_Y * 3];

const uint32_t SPI_SPEED_HZ = 20000000; // 20 MHz

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

}

/**
 * MARK: Looping
 */
void loop()
{
    // Make random output buffer
    for (size_t i = 0; i < NUM_LEDS_X * NUM_LEDS_Y * 3; ++i)
    {
        out_buf[i] = rand();
    }

#ifdef SPI_HAS_TRANSACTION
    std::cout << "Using new SPI library syntax" << std::endl;

    // Print the data to send
    // std::cout << "Data to send: " << std::endl;
    // for (size_t i = 0; i < NUM_LEDS_X * NUM_LEDS_Y * 3; i += 3)
    // {
    //     uint32_t color = out_buf[i] << 16 | out_buf[i + 1] << 8 | out_buf[i + 2];
    //     std::cout << "0x" << std::hex << std::setw(6) << std::setfill('0') << color << " ";

    //     // Print a newline every 16 colors
    //     if ((i + 3) % 16 * 3== 0)
    //     {
    //         std::cout << std::endl;
    //     }
    // }
    // std::cout << std::endl;

    long start = millis();

    vspi->beginTransaction(SPISettings(SPI_SPEED_HZ, MSBFIRST, SPI_MODE0));
    // Send data
    for (size_t i = 0; i < NUM_LEDS_X * NUM_LEDS_Y * 3; ++i)
    {
        vspi->write(out_buf[i]);
    }
    vspi->endTransaction();

    long elapsed = millis() - start;

    std::cout << "Bytes: " << std::dec << (NUM_LEDS_X * NUM_LEDS_Y * 3) << std::endl;
    std::cout << "Elapsed time: " << std::dec << elapsed << " ms" << std::endl;
    
    if (elapsed > 0)
    {
        // KiloBytes per second
        double kb_per_second = (NUM_LEDS_X * NUM_LEDS_Y * 3) / (elapsed / 1000.0) / 1024;
        std::cout << "KB per second: " << std::dec << kb_per_second << std::endl;
    } else {
        std::cout << "Elapsed time is too short to calculate KB per second" << std::endl;
    }
#else
    // Error cause this isn't supported
    std::cout << "Using old SPI library syntax" << std::endl;
    std::cout << "Please use the new SPI library syntax" << std::endl;
    exit(1);
#endif

    std::cout << "Looping..." << std::endl;
    delay(2000);
}

#endif
