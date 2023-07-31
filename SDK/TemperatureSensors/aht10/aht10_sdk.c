/**
 * @file aht10_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Sensor de Temperatura AHT10
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
#include "hardware/i2c.h"

// Conexoes do Sensor
#define I2C_ID        i2c0
#define I2C_SCL_PIN   17
#define I2C_SDA_PIN   16

#define BAUD_RATE     400000   // fast-mode 400KHz

// Endereco I2C do AHT10
#define AHT10_ADDR   0x38  

// Comandos AHT10
uint8_t cmdInit[] = { 0xE1, 0x08, 0x00 };
uint8_t cmdConv[] = { 0xAC, 0x33, 0x00 };

// Le status
int8_t getStatus ()  {  
  uint8_t val[1];  
    
  i2c_read_blocking (I2C_ID, AHT10_ADDR, val, 1, false);
  return val[0];  
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

  // Inicia interface I2C 
  uint baud = i2c_init (I2C_ID, BAUD_RATE);
  printf ("I2C @ %u Hz\n", baud);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SCL_PIN);
  gpio_pull_up(I2C_SDA_PIN);

  // Verifica se precisa calibrar
  uint8_t status = getStatus();
  if ((status & 0x08) == 0) {
    printf ("Calibrando\n");
    i2c_write_blocking (I2C_ID, AHT10_ADDR, cmdInit, 
                        sizeof(cmdInit), false);
    sleep_ms(10);
  }    

  // Laco Principal
  while(1) {
    // Dispara conversao
    i2c_write_blocking (I2C_ID, AHT10_ADDR, cmdConv, 
                        sizeof(cmdConv), false);
    // Aguarda conversao
    sleep_ms(80);
    // Pega resultado e mostra
    uint8_t r[6];
    i2c_read_blocking (I2C_ID, AHT10_ADDR, r, sizeof(r), false);
    float humid = (r[1] << 12) + (r[2] << 4) + (r[3] >> 4);
    humid = (humid / 0x100000) * 100.0;
    float temp = ((r[3] & 0x0F) << 16) + (r[4] << 8) + r[5];
    temp = (temp / 0x100000) * 200.0 - 50.0;
    printf("Temperatura: %.1fC Umidade: %.0f%%\n", temp, humid);  
    sleep_ms(2000);
  }
}
