/**
 * @file pir_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Sensor de Presenca (PIR)
 * @version 1.0
 * @date 2023-04-19
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


#define SENSOR_PIN 16

// Programa Principal
int main() {
    // Init stdio
    stdio_init_all();
    #ifdef LIB_PICO_STDIO_USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    #endif

    printf("\nExemplo sensor PIR\n");

    // Inicia sensor
    gpio_init (SENSOR_PIN);

    // Laco principal
    int counter = 0;
    while(1) {
      printf ("Sensor disparado %d vezes\n", counter);
      while (gpio_get(SENSOR_PIN) == 0)
        ;
      while (gpio_get(SENSOR_PIN) != 0)
        ;
      counter++;
    }
}