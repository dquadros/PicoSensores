/**
 * @file lm35_tmp36_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com os Sensores de Temperatura LM35D e TMP36
 * @version 1.0
 * @date 2023-06-05
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Pinos  (-1 se nao usado)
#define LM35_PIN 26
#define TMP36_PIN 27

// retorna a tensao em um pino ADC
// usa a media de 10 leituras
#define N_READINGS 10
float readSensor(int pin) {
  if (pin == -1) {
    return 0;
  }

  // pin 26 -> channel 0, pin 27 -> channel 1
  // pin 28 -> channel 2, pin 29 -> channel 3
  adc_select_input(pin-26);

  uint32_t sum = 0;
  for (int i = 0; i < N_READINGS; i++) {
    sum += adc_read();
  }
  return (sum*3.3 / N_READINGS)/4096.0;;
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

  // Inicia ADCs dos sensores
  adc_init();
  if (LM35_PIN != -1) {
    adc_gpio_init(LM35_PIN);
  }
  if (TMP36_PIN != -1) {
    adc_gpio_init(TMP36_PIN);
  }

  // Laco Principal
  while(1) {
    float tempLM35 = readSensor(LM35_PIN)/0.01;
    float tempTMP36 = 25.0 + (readSensor(TMP36_PIN)-0.75)/0.01;
    printf("LM35 = %.2fC  TMP36 = %.2fC\n", tempLM35, tempTMP36);
    sleep_ms(2000);
  }
}
