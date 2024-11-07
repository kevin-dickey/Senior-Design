#include <SPI_Impl.h>

SPIClass *ESPSPI::begin()
{
    if (this->verbose)
    {
        std::cout << "MOSI Pin: ";
        std::cout << VSPI_MOSI << std::endl;
        std::cout << "MISO Pin: ";
        std::cout << VSPI_MISO << std::endl;
        std::cout << "SCK Pin: ";
        std::cout << VSPI_SCK << std::endl;
        std::cout << "Pi Pico SS Pin: ";
        std::cout << VSPI_PICO_SS << std::endl;
        std::cout << "SD Card SS Pin: ";
        std::cout << VSPI_SD_SS << std::endl;
    }

    // Initialize the SPI bus
    vspi = new SPIClass(VSPI);
    vspi->begin(VSPI_SCK, VSPI_MISO, VSPI_MOSI, VSPI_PICO_SS);

    pinMode(VSPI_PICO_SS, OUTPUT); // VSPI SS
    pinMode(VSPI_SD_SS, OUTPUT);   // SD Card SS

    // Set the SS pin as an output
    pinMode(VSPI_PICO_SS, OUTPUT);
    digitalWrite(VSPI_PICO_SS, HIGH); // Deselect the pico for data transfer

    this->vspi = vspi;
    return vspi;
}

ESPSPI::~ESPSPI()
{
    this->vspi->end();
}

void ESPSPI::sendFrameData(uint8_t *out_buf, uint8_t *in_buf, size_t frameSize)
{
    // Select the pico for data transfer
    digitalWrite(VSPI_PICO_SS, LOW);

    this->vspi->beginTransaction(SPISettings(SPI_SPEED_HZ, MSBFIRST, SPI_MODE0));

    // Send a header of 0xF0 to indicate frame data in this transaction.
    uint8_t status = this->vspi->transfer(0xF0);

    delay(5);

    // Send data
    this->vspi->transferBytes(out_buf, in_buf, frameSize);

    this->vspi->endTransaction();

    // Deselect the pico for data transfer
    digitalWrite(VSPI_PICO_SS, HIGH);
}