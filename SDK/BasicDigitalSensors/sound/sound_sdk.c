/**
 * @file sound_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Sensor de Som
 * @version 1.0
 * @date 2023-04-19
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


#define LED_PIN 25
#define SENSOR_PIN 16

// Programa Principal
int main() {
    // Inicia LED
    gpio_init (LED_PIN);
    gpio_set_dir (LED_PIN, true);
    gpio_put (LED_PIN, false);

    // Inicia sensor
    gpio_init (SENSOR_PIN);

    // Laci principal
    while(1) {
      if (gpio_get(SENSOR_PIN) == 1) {
        gpio_put (LED_PIN, true);
        sleep_ms (500);
        gpio_put (LED_PIN, false);
      }
    }
}