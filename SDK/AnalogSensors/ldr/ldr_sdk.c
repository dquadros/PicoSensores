/**
 * @file ldr_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com LDR
 * @version 1.0
 * @date 2023-05-27
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Pinos
#define BUZZER_PIN 15
#define SENSOR_PIN 26

// Limite claro/escuro
#define DARK 300


// Programa Principal
int main() {
    // Inicia GPIO do Buzzer 
    gpio_init (BUZZER_PIN);
    gpio_set_dir (BUZZER_PIN, true);
    gpio_put (BUZZER_PIN, false);

    // Inicia ADC do sensor
    adc_init();
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(0);

    // Laco Principal
    while(1) {
      // Dorme por 3 a 60 segundos
      uint delay = (rand() % 57000) + 3000;
      sleep_ms (delay);
      // Bipe curto se escuro
      uint16_t val = adc_read(); // 0-4095
      if (val < DARK) {
        gpio_put (BUZZER_PIN, true);
        sleep_ms (100);
        gpio_put (BUZZER_PIN, false);
      }
    }
}
