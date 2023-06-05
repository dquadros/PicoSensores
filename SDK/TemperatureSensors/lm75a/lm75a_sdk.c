/**
 * @file lm75a_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Sensor de Temperatura LM75A
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

// Endereco I2C do LM75A
#define LM75A_ADDR    0x48  
  
// Registradores do LM75A
#define REG_TEMP  0  
#define REG_CONF  1  
#define REG_THYST 2  
#define REG_TOS   3  
  
//Escreve valor de 8 bits em um registrador
void WriteReg8 (uint8_t reg, int8_t val) {  
  uint8_t buffer[2];

  buffer[0] = reg;
  buffer[1] = val;
  i2c_write_blocking (I2C_ID, LM75A_ADDR, buffer, 2, false);
}  
  
//Escreve valor de 16 bits em um registrador
void WriteReg16 (uint8_t reg, int16_t val)  {  
  uint8_t buffer[3];

  buffer[0] = reg;
  buffer[1] = val >> 8;
  buffer[2] = val & 0xFF;
  i2c_write_blocking (I2C_ID, LM75A_ADDR, buffer, 3, false);
}  
  
// Le valor de 16 bits de um registrador
int16_t ReadReg16 (uint8_t reg)  {  
  uint8_t val[2];  
    
  // Seleciona registrador
  i2c_write_blocking (I2C_ID, LM75A_ADDR, &reg, 1, true);

  // Le valor
   i2c_read_blocking (I2C_ID, LM75A_ADDR, val, 2, false);
  return ((int16_t) val[0] << 8) | val[1];  
} 

// Codifica temperatura para o sensor
int16_t EncodeTemp (float temp)  {  
  if (temp >= 0) {
    return ((int16_t) (temp / 0.5)) << 7;
  }  else {
    return (512 + (int16_t) (temp / 0.5)) << 7;
  }
}  
  
// Decodifica temperatura informada pelo sensor
float DecodeTemp (int16_t val)  {  
  val = val / 32;
  if (val >= 1024) {
    return ((float) (val-2048)) * 0.125;
  } else {
    return ((float) val) * 0.125;
  }
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

  // Configura o sensor e define limites para a saida OS 
  WriteReg8 (REG_CONF, 0);
  WriteReg16 (REG_TOS, EncodeTemp(22.5));  
  WriteReg16 (REG_THYST, EncodeTemp(20.0));  

  // Laco Principal
  while(1) {
    sleep_ms(500);
    printf("Temperatura: %.3fC\n", DecodeTemp(ReadReg16(REG_TEMP)));  
  }
}
