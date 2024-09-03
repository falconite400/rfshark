/*
    CC1101 driver for the Raspberry Pi Pico using the C SDK

*/

#ifndef _CC1101_H_
#define _CC1101_H_
#include "hardware/spi.h"
#include "pico/stdlib.h"

#define SINGLE_BYTE_TX_FIFO 0x3f // register address for single byte tx fifo
#define BURST_ACCESS_TX_FIFO 0x7f // address for burst access tx fifo
#define SINGLE_BYTE_RX_FIFO 0xbf // address for single byte rx fifo
#define BUSRT_ACCESS_RX_FIFO 0xff // address for burst access rx fifo


typedef struct {
    spi_inst_t *spi_bus;
    char cs_pin;
} CC1101;


/*
    Initialize the CC1101 for operation.
    Arguments are the gpio number for each pin that will be used for
    the cc1101's spi bus. This function must be called for each
    cc1101 module connected to the pico, for a maximum of 2 cc1101
    modules connected over spi (the pico sdk provides 2 spi instances).
    Use pio for more spi instances if more cc1101 modules are needed.

    num should be 0 for the first cc1101 and 1 for the second. It ensures
    that the two modules can use separate spi interfaces on othe pico.
*/
CC1101 *cc1101_init(char sck, char tx, char rx, char cs, int num);


void write_reg(CC1101 *cc1101, uint8_t reg_addr, uint8_t **data, bool burst);
void read_reg(CC1101 *cc1101, uint8_t reg_addr, uint8_t **output, bool burst);



/* Get the part number for the CC1101. Address 0x30
    spi_inst_t *spi: the spi identifer for a spi bus
    uint8_t data: the buffer the read data will be stored
*/
void get_part_num(CC1101 *cc1101, uint8_t **output);


#endif