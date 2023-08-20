/**
 * @file fingerprint_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo de Uso do Sensor de Digitais
 * @version 1.0
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "fpm10a_sdk.h"

// Conexoes do sensor
#define UART_ID       uart0
#define UART_RX_PIN   17
#define UART_TX_PIN   16

// Conexões do LED RGB
#define LED_R_PIN     13
#define LED_G_PIN     14
#define LED_B_PIN     15
#define LED_MASK      ((1 << LED_R_PIN) | (1 << LED_G_PIN) | (1 << LED_B_PIN))

FPM10A *sensor;

// Le digital e gera feature
void captureFeature(uint8_t numbuf) {
  while (true) {
    gpio_put (LED_B_PIN, true);
    printf ("Coloque o dedo no sensor\n");
    while (!sensor->capture()) {
      sleep_ms(10);
    }
    gpio_put (LED_B_PIN, false);
    printf("Imagem capturada\n");

    bool ok = sensor->createFeature(numbuf);
    printf ("Remova o dedo do sensor\n");
    sleep_ms(2000);
    if (ok) {
      printf ("Feature criada\n");
      return;
    }
    gpio_put (LED_R_PIN, true);
    printf ("Imagem ruim, tente de novo\n");
    sleep_ms(1000);
    gpio_put (LED_R_PIN, false);
  }
}

// Cadastra Digital
void enroll() {
  bool first = true;
  while (true) {
    if (!first) {
      captureFeature(1);
    }
    captureFeature(2);
    if (!sensor->createModel()) {
      gpio_put (LED_R_PIN, true);
      printf ("Imagens nao combinam, vamos repetir\n");
      sleep_ms(1000);
      gpio_put (LED_R_PIN, false);
      continue;
    }
    int pos = sensor->count();
    if (sensor->store(1, pos)) {
      gpio_put (LED_G_PIN, true);
      printf ("Digital cadastrada na posicao %d\n", pos);
      sleep_ms(1000);
      gpio_put (LED_G_PIN, false);
      return;
    }
    gpio_put (LED_R_PIN, true);
    printf ("Erro %d ao salvar digital\n", sensor->lastResponse());
    sleep_ms(1000);
    gpio_put (LED_R_PIN, false);
  }
}

// Programa Principal
int main() {
  // Init stdio
  stdio_init_all();
  #ifdef LIB_PICO_STDIO_USB
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  #endif
  printf ("\nExemplo Sensor Digitais\n\n");

  // Inicia LED
  gpio_init_mask (LED_MASK);
  gpio_set_dir_masked (LED_MASK, LED_MASK);
  gpio_put_masked (LED_MASK, 0);

  // Inicia interface serial
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Inicia sensor
  sensor = new FPM10A(UART_ID);
  sensor->begin();

  // Informa a quantidade de digitais que podem ser armazenadas
  printf ("Verificando capacidade do sensor\n");
  FPM10A::SYSPARAM sp;
  if (sensor->leSysParam(&sp)) {
    printf ("Sensor pode armazenar ate %d digitais\n", 
      (sp.libsize[0] << 8) + sp.libsize[1]);
  }

  // Limpa digitais armazenadas (se tiver)
  int count = sensor->count();
  if (count > 0) {
    printf ("Apagando %d digitais\n", count);
    if (sensor->clear()) {
      printf ("Sucesso\n");
    }
  }

  // Main loop
  while(1) {
    printf("\n");
    captureFeature(1);
    printf("Procurando...\n");
    int pos = sensor->search(1);
    if (pos == -1) {
      printf ("Desconhecida, vamos cadastrar\n");
      enroll();
    } else {
      gpio_put (LED_G_PIN, true);
      printf ("Digital %d reconhecida\n", pos);
      sleep_ms(1000);
      gpio_put (LED_G_PIN, false);
    }
  }
}
