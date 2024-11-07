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
#include "blink.pio.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#ifndef SPI_PORT
#define SPI_PORT spi0
#endif

#define IS_RGBW false
#define BUILTIN_LED 25
#define LED_PIN 2
#define NUM_LEDS_X 5 
#define NUM_LEDS_Y 1
#define LED_BYTES 3

#define BUF_LEN NUM_LEDS_X * NUM_LEDS_Y * LED_BYTES

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           (uint32_t)(b);
}

static inline uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           ((uint32_t)(w) << 24) |
           (uint32_t)(b);
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

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq)
{
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    printf("Blinking pin %d at %d Hz\n", pin, freq);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    pio->txf[sm] = (125000000 / (2 * freq)) - 3;
}

void handle_control_message()
{
}

int main()
{
    stdio_init_all();

    // Wait for serial monitor connection
    sleep_ms(7000);

    // PIO Blinking ===========================================================
    PIO pio_1 = pio1;
    int state_machine_1 = 0;
    uint offset1 = pio_add_program(pio1, &blink_program);
    printf("Loaded blink program at %d\n", offset1);

    // PIO WS2812 ===========================================================
    PIO pio_0 = pio0;
    int state_machine_0 = 0;
    uint offset0 = pio_add_program(pio0, &ws2812_program);
    printf("Loaded ws2812 program at %d\n", offset0);

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
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI));

    // DMA Setup ===============================================================
    // Grab an unused DMA channel for SPI Received Data
    int spi_dma_channel_rx = dma_claim_unused_channel(true);
    int spi_dma_channel_tx = dma_claim_unused_channel(true);

    // Set up the DMA to receive data into in_buf as it comes in on the SPI line
    // https://github.com/raspberrypi/pico-examples/blob/7e77a0c381863be0c49086567e7f1934d78ac591/spi/spi_dma/spi_dma.c#L68-L80
    printf("Setting up DMA channel %d to receive data from SPI\n", spi_dma_channel_rx);
    dma_channel_config c = dma_channel_get_default_config(spi_dma_channel_rx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq(spi_default, false));
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);

    // Sends status bytes from the spi_out_byte, doesn't write to memory. Just sends it out
    printf("Setting up DMA channel %d to send data to SPI\n", spi_dma_channel_tx);
    dma_channel_config c_tx = dma_channel_get_default_config(spi_dma_channel_tx);
    channel_config_set_transfer_data_size(&c_tx, DMA_SIZE_8);
    // This might be wrong
    channel_config_set_dreq(&c_tx, spi_get_dreq(spi_default, true));
    channel_config_set_read_increment(&c_tx, false);
    channel_config_set_write_increment(&c_tx, false);

    // Program State ===============================================================
    uint8_t header_buf;
    uint8_t in_buf[BUF_LEN];
    uint8_t spi_out_byte = 0xFF;

#ifdef PICO_DEFAULT_LED_PIN
    blink_pin_forever(pio_1, state_machine_1, offset1, PICO_DEFAULT_LED_PIN, 2);
#else
    blink_pin_forever(pio_1, state_machine_1, offset1, BUILTIN_LED, 3);
#endif

    ws2812_program_init(pio_0, state_machine_0, offset0, LED_PIN, 800000, IS_RGBW);

    while (true)
    {
        dma_channel_abort(spi_dma_channel_rx);
        dma_channel_abort(spi_dma_channel_tx);

        // Clear the buffers - might not be needed
        memset(&header_buf, 0, sizeof(header_buf));
        memset(&in_buf, 0, sizeof(in_buf));

        // Configure SPI DMA to send status messages to the SPI master
        dma_channel_configure(
            spi_dma_channel_tx,
            &c_tx,
            &spi_get_hw(SPI_PORT)->dr,  // Destination
            &spi_out_byte,              // Source
            BUF_LEN + 1,                // Number of transfers
            false                       // Wait to start
        );

        // Start the DMA channel to receive the 1 control byte from SPI
        dma_channel_configure(spi_dma_channel_rx,
                              &c,
                              &header_buf,
                              &spi_get_hw(SPI_PORT)->dr,
                              1,
                              false);

        uint32_t wait_start = to_ms_since_boot(get_absolute_time());

        // Start the DMA channel and wait for it to finish
        printf("Starting RX DMA channel %d and TX DMA channel %d\n", spi_dma_channel_rx, spi_dma_channel_tx);
        dma_start_channel_mask((1u << spi_dma_channel_tx) | (1u << spi_dma_channel_rx));
        // dma_start_channel_mask((1u << spi_dma_channel_rx));

        printf("Waiting for data from SPI...\n");
        dma_channel_wait_for_finish_blocking(spi_dma_channel_rx);

        // Read the control byte
        printf("Control byte: 0x%02x\n", header_buf);
        if (header_buf == 0xC0)
        {
            printf("Received 0xC0, setting control parameters.\n");
            // dma_channel_configure(
            //     spi_dma_channel_rx,
            //     &c,
            //     in_buf,                    // Destination
            //     &spi_get_hw(SPI_PORT)->dr, // Source
            //     BUF_LEN,                   // Number of transfers
            //     true                       // Don't start immediately
            // );

            handle_control_message();
        } 
        else if (header_buf == 0xF0)
        {
            printf("Received 0xF0, interpreting the next %d bytes as frame data!\n", BUF_LEN);
            dma_channel_configure(
                spi_dma_channel_rx,
                &c,
                &in_buf,                   // Destination
                &spi_get_hw(SPI_PORT)->dr, // Source
                BUF_LEN,                   // Number of transfers
                true                       // Don't start immediately
            );
        }

        // Wait for the DMA channel to finish
        dma_channel_wait_for_finish_blocking(spi_dma_channel_rx);
        printf("DMA channel %d finished in %ld ms\n", spi_dma_channel_rx, to_ms_since_boot(get_absolute_time()) - wait_start);

        dma_channel_wait_for_finish_blocking(spi_dma_channel_tx);
        printf("DMA channel %d finished in %ld ms\n", spi_dma_channel_tx, to_ms_since_boot(get_absolute_time()) - wait_start);

        printf("Done reading from SPI.\n");
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
    dma_channel_unclaim(spi_dma_channel_tx);
    // spi_deinit(SPI_PORT);
#endif
}
