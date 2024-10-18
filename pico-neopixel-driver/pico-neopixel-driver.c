#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/binary_info.h"
#include "hardware/clocks.h"
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#include "ws2812.pio.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#ifndef SPI_PORT
#define SPI_PORT spi0
#endif

#define PIN_MISO 16
#define PIN_CS 17
#define PIN_SCK 18
#define PIN_MOSI 19


#define IS_RGBW false
#define LED_PIN 20
#define NUM_LEDS_X 60
#define NUM_LEDS_Y 1
#define LED_BYTES 3

#define BUF_LEN NUM_LEDS_X * NUM_LEDS_Y * LED_BYTES

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

static inline uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            ((uint32_t) (w) << 24) |
            (uint32_t) (b);
}

void printbuf(uint8_t buf[], size_t len)
{
    printf("Data received:\n");
    for (size_t i = 0; i < len; i += 3)
    {
        uint32_t color = (buf[i] << 16) | (buf[i + 1] << 8) | buf[i + 2];
        printf("0x%06x ", color);

        // Print a newline every 16 colors
        if ((i / 3 + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

int main()
{
    stdio_init_all();

    // Wait for serial monitor connection
    sleep_ms(7000);

    // PIO Blinking ===========================================================
    PIO pio = pio0;
    int state_machine = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    printf("Loaded program at %d\n", offset);

    // SPI Slave ===============================================================
#if !defined(SPI_PORT) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_RX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/spi_slave example requires a board with SPI pins
    puts("Default SPI pins were not defined");
#else

    printf("SPI slave example\n");
    printf("Setting up SPI slave on pins:\n");
    printf("SCK: %d\n", PICO_DEFAULT_SPI_SCK_PIN);
    printf("MOSI: %d\n", PICO_DEFAULT_SPI_RX_PIN);
    printf("MISO: %d\n", PICO_DEFAULT_SPI_TX_PIN);
    printf("CSN: %d\n", PICO_DEFAULT_SPI_CSN_PIN);

    // Enable SPI 0 at 1 MHz and connect to GPIOs
    spi_init(SPI_PORT, 1 * 1000 * 1000);
    spi_set_slave(SPI_PORT, true);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);

    // THIS LINE IS ABSOLUTELY KEY FOR SPI. Enables multi-byte transfers with one CS assert
    // Page 537 of the RP2040 Datasheet. Specific mode of the Motorola-esque SPI controller
    spi_set_format(SPI_PORT, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    gpio_set_dir(PICO_DEFAULT_SPI_TX_PIN, GPIO_OUT);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI));

    // Grab an unused DMA channel for SPI Received Data
    int spi_dma_channel_rx = dma_claim_unused_channel(true);

    uint8_t in_buf[BUF_LEN];

    // Set up the DMA to receive data into in_buf as it comes in on the SPI line
    // https://github.com/raspberrypi/pico-examples/blob/7e77a0c381863be0c49086567e7f1934d78ac591/spi/spi_dma/spi_dma.c#L68-L80
    printf("Setting up DMA channel %d to receive data from SPI\n", spi_dma_channel_rx);
    dma_channel_config c = dma_channel_get_default_config(spi_dma_channel_rx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq(spi_default, false));
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);

    ws2812_program_init(pio, state_machine, offset, LED_PIN, 800000, IS_RGBW);

    while (true)
    {
        dma_channel_abort(spi_dma_channel_rx);
        dma_channel_configure(
            spi_dma_channel_rx,
            &c,
            in_buf,                    // Destination
            &spi_get_hw(SPI_PORT)->dr, // Source
            BUF_LEN,                   // Number of transfers
            false                      // Don't start immediately
        );
        
        // Start the DMA channel
        printf("Starting DMA channel %d\n", spi_dma_channel_rx);
        // dma_channel_start(spi_dma_channel_rx);
        dma_start_channel_mask(1u << spi_dma_channel_rx);

        printf("Waiting for data from SPI...\n");

        uint32_t wait_start = to_ms_since_boot(get_absolute_time());

        // Wait for the DMA channel to finish
        dma_channel_wait_for_finish_blocking(spi_dma_channel_rx);
        
        printf("DMA channel %d finished in %ld ms\n", spi_dma_channel_rx, to_ms_since_boot(get_absolute_time()) - wait_start);

        printf("Done reading from SPI. Data:\n");
        printbuf(in_buf, BUF_LEN);

        // Send the data to the PIO state machine
        for (size_t i = 0; i < BUF_LEN; i += 3)
        {
            uint32_t color = (in_buf[i] << 16) | (in_buf[i + 1] << 8) | in_buf[i + 2];
            put_pixel(color);
        }
    }

    // Clean up
    dma_channel_unclaim(spi_dma_channel_rx);
    // spi_deinit(SPI_PORT);
#endif
}
