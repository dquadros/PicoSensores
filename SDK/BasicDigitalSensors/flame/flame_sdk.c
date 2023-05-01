/**
 * @file flame_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Sensor de Chama
 * @version 1.0
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


#define BUZZER_PIN 15
#define SENSOR_PIN 16

// Programa principal
int main() {
    // Inicia buzzer
    gpio_init (BUZZER_PIN);
    gpio_set_dir (BUZZER_PIN, true);
    gpio_put (BUZZER_PIN, false);

    // Inicia sensor
    gpio_init (SENSOR_PIN);

    // Laco principal
    while(1) {
      if (gpio_get(SENSOR_PIN) == 0) {
        gpio_put (BUZZER_PIN, true);
        sleep_ms (500);
        gpio_put (BUZZER_PIN, false);
      }
    }
}