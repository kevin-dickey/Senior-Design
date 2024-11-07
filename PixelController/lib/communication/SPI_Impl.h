#include <iostream>

#include <Arduino.h>
#include <SPI.h>

#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCK 18
#define VSPI_PICO_SS 5
#define VSPI_SD_SS 17

const uint32_t SPI_SPEED_HZ = 1 * 1000 * 1000; // 1 MHz

class ESPSPI
{
public:
    ESPSPI(bool verbose = false) : verbose(verbose) {};
    ~ESPSPI();

    SPIClass* vspi;

    SPIClass* begin();
    void sendFrameData(uint8_t *out_buf, uint8_t *in_buf, size_t size);

private:
    bool verbose = false;
};


