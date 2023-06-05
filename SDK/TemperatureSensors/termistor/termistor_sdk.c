/**
 * @file termistor_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Termistor
 * @version 1.0
 * @date 2023-06-05
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Conexao do termistor
#define THERMISTOR_PIN 26

// Parametros do circuito
const float Vcc = 3.3;  
const float R = 2200.0; 

// Referencia
typedef struct {
  float r;
  float t;
} ResTemp;

// Retorna a resistencia do termistor
// usa a media de 20 leituras do ADC
float getResistance() {
  uint32_t sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += adc_read();
  }
  float v = (Vcc*sum)/(20*4096.0);
  return R*(Vcc-v)/v;
}

// Obtem uma temperatura de referencia
void getReference(ResTemp *ref) {
  printf("Temperatura: ");
  scanf("%f", &ref->t);
  printf("%.1f", ref->t);
  ref->t += 273.0;
  ref->r = getResistance();
  printf(" Resistencia: %.0f\n", ref->r);
}

// Programa Principal
int main() {
  ResTemp ref1, ref2;
  float rx, beta;

  // Inicia stdio
  stdio_init_all();
  #ifdef LIB_PICO_STDIO_USB
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  #endif

  // Inicia ADC do sensor
  adc_init();
  adc_gpio_init(THERMISTOR_PIN);
  adc_select_input(THERMISTOR_PIN-26);

  // Obtem duas referencias
  printf("Referencia 1\n");
  getReference (&ref1);
  printf("Referencia 2\n");
  getReference (&ref2);

  // Calcula beta e rx
  beta = log(ref1.r/ref2.r)/((1/ref1.t)-(1/ref2.t));
  printf("Beta = %.2f\n", beta);
  rx = ref1.r * exp(-beta/ref1.t);

  // Laco Principal
  while(1) {
    float rt = getResistance();
    float t = beta / log(rt/rx);
    printf("Temperatura: %.1f\n", t-273.0);
    sleep_ms(1000);
  }
}
