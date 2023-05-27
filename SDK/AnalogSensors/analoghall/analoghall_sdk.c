/**
 * @file analoghall_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Sensor Efeito Hall Analogico
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

// Leitura sem campo magnetico
int zero;

// Le o sensor (media de 50 leituras)
#define N_READINGS 50
uint readSensor() {
  uint32_t sum = 0;
  for (int i = 0; i < N_READINGS; i++) {
    sum += adc_read();
  }
  return sum / N_READINGS;
}


// Programa Principal
int main() {
  // Inicia stdio
  stdio_init_all();
  #ifdef LIB_PICO_STDIO_USB
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  #endif

  // Inicia ADC ligado ao sensor 
  adc_init();
  adc_gpio_init(SENSOR_PIN);
  adc_select_input(0);

  // Assume que nao tem campo magnetico no inicio
  zero = readSensor() ;
  printf("Zero = %d\n", zero);
  sleep_ms(1000);
  printf ("Pronto\n");

  // Laco Principal
  while(1) {
    int field = readSensor();
    if (((field-zero) < 30) && ((field-zero) > -30)) {
      printf ("Sem campo\n");
    } else if (field > zero) {
      printf ("Campo = S %d\n", field-zero);
    } else {
      printf ("Campo = N %d\n", zero-field);
    }
    sleep_ms(1000);
  }
}
