/*
    CC1101 driver for the Raspberry Pi Pico using the C SDK

*/
/*
All transactions on the SPI interface start with
a header byte containing a R/W bit, a burst
access bit (B), and a 6-bit address (A5 – A0).
*/
#include "cc1101.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>


CC1101 *cc1101_init(char sck, char tx, char rx, char cs, int num) {

    CC1101 *cc1101 = malloc(sizeof(CC1101));
    if (num == 0) {
        cc1101->spi_bus = spi0;
    } else if (num == 1) {
        cc1101->spi_bus = spi1;
    } else {
        return NULL;
    }

    spi_init(cc1101->spi_bus, 500000);
    
    // spi interface, 8 data bits per transfer, CPOL
    // polarity and CPHA phase both 0, must be SPI_MSB_FIRST
    spi_set_format(cc1101->spi_bus, 8, 0, 0, SPI_MSB_FIRST);

    // initialize the other SPI pins
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(tx, GPIO_FUNC_SPI);
    gpio_set_function(rx, GPIO_FUNC_SPI);

    // set the CS pin high
    cc1101->cs_pin = cs;
    gpio_init(cc1101->cs_pin);
    gpio_set_dir(cc1101->cs_pin, GPIO_OUT);
    gpio_put(cc1101->cs_pin, 1);

    return cc1101;
}


void write_reg(CC1101 *cc1101, uint8_t reg_addr, uint8_t **data, bool burst) {

    // header byte to send to the cc1101. the first bit is read/write (1/0),
    // the second bit is burst access (used to access consecutive registers
    // more efficiently, and the last 6 are address bits. burst access is
    // activated with a 1 bit. if the burst access bit is 0, the cc1101 expects
    // one data byte afterward
    uint8_t header_byte;

    // write CS low
    gpio_put(cc1101->cs_pin, 0);

    // Set the header byte. First bit is R/W (1/0), second is burst access
    // (used to access consecutive registers more efficiently), next six are
    // address bits
    
    //test
    // ensure that the first two bits of reg_addr are 0, while doing nothing
    // to the six address bits
    reg_addr &= 0b00111111;

    if (burst) {
        // burst address bit (second bit) needs to be set to 1.
        // combine register address with first two bits of header byte
        // (which are read (0) and burst (1)
        header_byte = 0b01000000 | reg_addr;

        // write header byte
        spi_write_blocking(cc1101->spi_bus, &header_byte, 1);
        // write each byte of data to consecutive addresses in tx fifo after reg_addr
        // (the currently written to address is automatically increased by the chip
        // on the module)
        for (int i = 0; i < (sizeof(data) / sizeof(*data[0])); i++) {
            printf("%d %c\n", i, *data[0]);
            spi_write_blocking(cc1101->spi_bus, data[i], 1);
        }

    } else {
        // single byte access.
        // same procedure for the header byte, except the second bit is zero to
        // indicate single byte access instead of burst access
        header_byte = 0b00000000 | reg_addr;
        // write the header byte to the cc1101
        spi_write_blocking(cc1101->spi_bus, &header_byte, 1);
        spi_write_blocking(cc1101->spi_bus, data[0], 1);
    }

    // pull chip select high, which will terminate burst access writing if it was
    // active.
    gpio_put(cc1101->cs_pin, 1);

}

void read_reg(CC1101 *cc1101, uint8_t reg_addr, uint8_t **output, bool burst) {

    uint8_t header_byte;
    uint8_t data;

    // ensure that the first two bits of reg_addr are 0, while doing nothing
    // to the six address bits
    reg_addr &= 0b00111111;

    if (burst) {
        // set header byte. first bit is read (1), second bit is burst access (1)
        header_byte = 0b11000000 | reg_addr;
        // write the header byte to the cc1101
        spi_write_blocking(cc1101->spi_bus, &header_byte, 1);
        // read from each of the 64 bytes in the rx fifo if the data is not empty
        for (int i = 0; i < 64; i++) {
            spi_read_blocking(cc1101->spi_bus, 0, output[i], 1);
        }
    }
}

void get_part_num(CC1101 *cc1101, uint8_t **output) {
    const uint8_t addr = 0x30;
    read_reg(cc1101, addr, output, 0);
}