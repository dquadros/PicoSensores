/**
 * @file mcp9808_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Sensor MCP9808
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

// Conexoes do sensor
#define I2C_ID        i2c0
#define I2C_SCL_PIN   17
#define I2C_SDA_PIN   16

#define BAUD_RATE     400000   // fast-mode 400KHz

// Endereco I2C do  MCP9808 
#define MCP9808_ADDR   0x18  
  
// Registradores do MCP9808
#define REG_CONFIG  1
#define REG_UPPER   2  
#define REG_LOWER   3
#define REG_CRIT    4
#define REG_TA      5
#define REG_MANID   6
#define REG_DEVID   7
#define REG_RESOL   8
  
// Escreve valor de 16 bits em um registrador
void WriteReg16 (uint8_t reg, int16_t val)  {  
  uint8_t buffer[3];

  buffer[0] = reg;
  buffer[1] = val >> 8;
  buffer[2] = val & 0xFF;
  i2c_write_blocking (I2C_ID, MCP9808_ADDR, buffer, 3, false);
}  
  
//Le registrador de 16 bits
int16_t ReadReg16 (uint8_t reg)  {  
  uint8_t val[2];  
    
  // Seleciona registrador
  i2c_write_blocking (I2C_ID, MCP9808_ADDR, &reg, 1, true);

  // Le valor
   i2c_read_blocking (I2C_ID, MCP9808_ADDR, val, 2, false);
  return ((int16_t) val[0] << 8) | val[1];  
} 

// Codifica temperatura para o sensor
int16_t EncodeTemp (float temp)  {  
  if (temp >= 0) {
    return ((uint16_t) (temp/0.25)) << 2;
  } else {
    return ((uint16_t) (2048 + temp/0.25)) << 2;
  }  
}  
  
// Decodifica temperatura lida do sensor
float DecodeTemp (int16_t val)  {  
  bool sign = (val & 0x1000) != 0;
  val = val & 0x1FFF;
  if (sign) {
    val = val - 0x2000;
  }
  return (float)val/16.0;
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

  // Verifica IDs de fabricante e dispositivo
  uint16_t manID = ReadReg16(REG_MANID);
  uint16_t devID = ReadReg16(REG_DEVID);
  printf ("Fabricante: %04X Dispositivo: %02X rev %u\n",
    manID, devID >> 8, devID & 0xFF);

  // Define limites para saida Alert
  WriteReg16(REG_CRIT,EncodeTemp(30.0));
  WriteReg16(REG_UPPER,EncodeTemp(23.0));
  WriteReg16(REG_LOWER,EncodeTemp(20.0));
  WriteReg16(REG_CONFIG,0x0008);

  // Laco principal: le temperatura
  while(1) {
    sleep_ms(500);
    printf("Temperatura: %.3fC\n", DecodeTemp(ReadReg16(REG_TA)));  
  }
}
