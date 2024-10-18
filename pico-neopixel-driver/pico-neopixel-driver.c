#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/pio.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define BUF_LEN  40 * 60 * 3

#include "blink.pio.h"


void printbuf(uint8_t buf[], size_t len) {
    printf("Data received:\n");
    for (size_t i = 0; i < len; i += 3) {
        uint32_t color = (buf[i] << 16) | (buf[i + 1] << 8) | buf[i + 2];
        printf("0x%06x ", color);

        // Print a newline every 16 colors
        if ((i / 3 + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq) {
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    printf("Blinking pin %d at %d Hz\n", pin, freq);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    pio->txf[sm] = (125000000 / (2 * freq)) - 3;
}



int main()
{
    stdio_init_all();

    // Wait for serial monitor connection
    sleep_ms(7000);

    // PIO Blinking ===========================================================
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blink_program);
    printf("Loaded program at %d\n", offset);
    
    #ifdef PICO_DEFAULT_LED_PIN
    blink_pin_forever(pio, 0, offset, PICO_DEFAULT_LED_PIN, 3);
    #else
    blink_pin_forever(pio, 0, offset, 6, 3);
    #endif
    // For more pio examples see https://github.com/raspberrypi/pico-examples/tree/master/pio

    // SPI Slave ===============================================================
#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_RX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/spi_slave example requires a board with SPI pins
    puts("Default SPI pins were not defined");
#else

    printf("SPI slave example\n");
    printf("Setting up SPI slave on pins:\n");
    printf("SCK: %d\n", PICO_DEFAULT_SPI_SCK_PIN);
    printf("MOSI: %d\n", PICO_DEFAULT_SPI_RX_PIN);
    printf("MISO: %d\n", PICO_DEFAULT_SPI_TX_PIN);
    printf("CSN: %d\n", PICO_DEFAULT_SPI_CSN_PIN);

    // Enable SPI 0 at 10 MHz and connect to GPIOs
    spi_init(spi_default, 20 * 1000 * 1000);
    spi_set_slave(spi_default, true);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);

    // THIS LINE IS ABSOLUTELY KEY. Enables multi-byte transfers with one CS assert
    // Page 537 of the RP2040 Datasheet.
    spi_set_format(spi_default, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST); 
    gpio_set_dir(PICO_DEFAULT_SPI_TX_PIN, GPIO_OUT);

    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI));

    uint8_t in_buf[BUF_LEN];

    for (size_t i = 0; ; ++i) {
        printf("SPI slave says: Waiting for a page to be read from the MOSI line...\n");

        // Write the output buffer to MISO, and at the same time read from MOSI.
        spi_read_blocking(spi_default, 0, in_buf, BUF_LEN);

        // Write to stdio whatever came in on the MOSI line.
        printf("SPI slave says: read page %d from the MOSI line:\n", i);
        printbuf(in_buf, BUF_LEN);
    }
#endif
}
