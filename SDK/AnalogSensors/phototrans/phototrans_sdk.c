/**
 * @file phototrans_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Fototransistor
 * @version 1.0
 * @date 2023-05-27
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Pinos
#define SENSOR_PIN 26

// Programa Principal
int main() {
    // Inicia stdio
    stdio_init_all();
    #ifdef LIB_PICO_STDIO_USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    #endif

    // Inicia ADC do sensor
    adc_init();
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(0);

    // Laco Principal
    uint16_t val_ant = 0;
    while(1) {
      uint16_t val = adc_read(); // 0-4095
      int dif = (val > val_ant)? val - val_ant : val_ant - val;
      if (dif > 300) {
        printf("%u\n", val);
        val_ant = val;
      }
    }
}
