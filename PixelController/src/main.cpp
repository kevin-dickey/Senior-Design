#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include <SPI.h>

#include "../include/json.hpp"
#include "../lib/configuration/Configuration.h"

using json = nlohmann::json;

#define LED_PIN 13
#define SWITCH_BUF_PIN 2
#define NUM_LEDS_X 5
#define NUM_LEDS_Y 1
#define MAX_BRIGHTNESS 64 // maximum for FastLED is 255, (don't go higher than like 8 if you don't have a PSU attached)

#if USE_EMULATOR

#define PROJECT_DIR SOURCE_ROOT

#else
#define PROJECT_DIR PROJECT_DIR

#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCK 18
#define VSPI_SS 5
#define VSPI_SD_SS 17

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
uint8_t in_buf[NUM_LEDS_X * NUM_LEDS_Y * 3];

const uint32_t SPI_SPEED_HZ = 1000000; // 1 MHz

void setup()
{
    Serial.begin(115200);                    // for setting up stuff to print to serial monitor
    std::cout << "Starting..." << std::endl; // print to the serial monitor that the program is starting

    Serial.print("MOSI Pin: ");
    Serial.println(VSPI_MOSI);
    Serial.print("MISO Pin: ");
    Serial.println(VSPI_MISO);
    Serial.print("SCK Pin: ");
    Serial.println(VSPI_SCK);
    Serial.print("Pi Pico SS Pin: ");
    Serial.println(VSPI_SS);
    Serial.print("SD Card SS Pin: ");
    Serial.println(VSPI_SD_SS);

    vspi = new SPIClass(VSPI);
    // Set the pins for SPI, Automatically pull Pi SS high.
    vspi->begin(VSPI_SCK, VSPI_MISO, VSPI_MOSI, VSPI_SS);

    pinMode(VSPI_SS, OUTPUT);    // VSPI SS
    pinMode(VSPI_SD_SS, OUTPUT); // SD Card SS
    pinMode(SWITCH_BUF_PIN, INPUT_PULLDOWN);

    // Set the device SS pins to high to set them to not be selected.
    // This is redundant on the VSPI_SS pin due to the declaration in the vspi->begin() statement above
    digitalWrite(VSPI_SS, HIGH);
    digitalWrite(VSPI_SD_SS, HIGH);

    std::cout << "Setup Complete..." << std::endl;

    delay(2000); // delay for 3 seconds to give time to open the serial monitor
}

/**
 * MARK: Looping
 */
void loop()
{
    // Make random output buffer
    for (size_t i = 0; i < NUM_LEDS_X * NUM_LEDS_Y * 3; ++i)
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
    for (size_t i = 0; i < NUM_LEDS_X * NUM_LEDS_Y * 3; ++i)
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
        digitalWrite(VSPI_SS, LOW); // Select the pico for data transfer

        vspi->beginTransaction(SPISettings(SPI_SPEED_HZ, MSBFIRST, SPI_MODE0));
        // Send a header of 0xF0 to indicate frame data in this transaction.
        uint8_t status = vspi->transfer(0xF0);

        delay(5);

        // Send data - DO NOT INCREMENT i IN THE FOR LOOP INITIALIZATION
        vspi->transferBytes(out_buf, in_buf, NUM_LEDS_X * NUM_LEDS_Y * 3);

        vspi->endTransaction();

        digitalWrite(VSPI_SS, HIGH); // Deselect the pico for data transfer

        std::cout << "Received control header status: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(status) << std::endl;

        // Print the data received from the pico
        std::cout << "Data received: ";
        for (size_t i = 0; i < NUM_LEDS_X * NUM_LEDS_Y * 3; ++i)
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
