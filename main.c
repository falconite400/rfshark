#include <stdio.h>

#include "hardware/spi.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "cc1101/cc1101.h"

#define CSN 5
#define SCK 2
#define TX 3
#define RX 4    


int main() {

    int spi_baudrate = 5600;


    stdio_init_all();

    // initialize the cc1101 modules
    CC1101 *cc1101_0 = cc1101_init(2, 3, 4, 5, 0);
    CC1101 *cc1101_1 = cc1101_init(6, 7, 8, 9, 1);

    if (cc1101_0 == NULL) {
        printf("Failed to allocate memory for cc1101_0");
        return -1;
    } else if (cc1101_1 == NULL) {
        printf("Failed to allocate memory for cc1101_1");
        return -1;
    }

    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed");
        return -1;
    }
    uint8_t *data[2];

    while (true) {
        get_part_num(cc1101_0, data);
        for (int i = 0; i < 2; i++) {
            size_t size = sizeof(data[i]);
            fwrite(data[i], 1, size, stdout);
            printf("\n");
            //printf("part num is %x\n", data[i]);
        }
        printf("Size of int is %zu\n", sizeof(int));
        /*
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(1000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(1000);
        */
    }
}