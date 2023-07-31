/**
 * @file adlx245_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com sensor ADLX345
 * @version 1.0
 * @date 2023-07-30
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Conexões do sensor
#define I2C_ID        i2c0
#define I2C_SCL_PIN   17
#define I2C_SDA_PIN   16

#define BAUD_RATE     400000   // 400kHz

// Classe para acessar o sensor
class ADLX345 {

  public:

    typedef struct {
      float x;
      float y;
      float z;
    } VECT_3D;

    // Endereço I2C Padrão
    static const uint8_t I2C_ADDR = 0x1D;

    // ID do dispositivo
    static const uint8_t ID = 0xE5;

    // Construtor
    ADLX345 (i2c_inst_t *i2c, uint8_t addr = ADLX345::I2C_ADDR);

    // Métodos públicos
    void    begin(void);
    uint8_t getId(void);
    void    getAccel(VECT_3D *vect, float scale = 256);

  private:
    
    // Endereços dos registradores
    static const int DEVID = 0x00;
    static const int POWER_CTL = 0x2D;
    static const int DATA_FORMAT = 0x31;
    static const int DATAX0 = 0x32;

    // Instrância do I2C 
    i2c_inst_t *i2c;

    // Endereço I2C do sensor
    uint8_t addr;

    // ID do sensor
    uint8_t id;

    // Dado bruto
    int16_t raw[3];

    // Rotinas privativas
    void     readRaw(void);
    uint8_t  read8(uint8_t reg);
    void     write8(uint8_t reg, uint8_t val);
};

// Construtor
ADLX345::ADLX345 (i2c_inst_t *i2c, uint8_t addr) {
  this->i2c = i2c;
  this->addr = addr;
}

// Configura sensor
void ADLX345::begin() {
  id = read8(DEVID);
  write8(POWER_CTL, 0x08);   // modo de medição
  write8(DATA_FORMAT, 0x00); // faixa 2g, 10-bit
}

// Retorna o ID do sensor
uint8_t ADLX345::getId() {
  return id;
}

// Obtem aceleração em g
void ADLX345::getAccel(VECT_3D *vect, float scale) {
  readRaw();
  vect->x = (float) raw[0] / scale;
  vect->y = (float) raw[1] / scale;
  vect->z = (float) raw[2] / scale;
}

// Le valores brutos
void ADLX345::readRaw() {
  uint8_t data[6];

  // Seleciona primeiro registrador
  uint8_t reg = DATAX0;
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Lê os valores
  i2c_read_blocking (i2c, addr, data, 6, false);

  // Converte para int16
  for (int i = 0; i < 3; i++) {
    raw[i] = (data[2*i+1] << 8) | data[2*i];
  }
}

// Lê valor de 8 bits de um registrador
uint8_t ADLX345::read8 (uint8_t reg)  {  
  uint8_t val[1];  
    
  // Seleciona o registrador
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Faz a leitura
  i2c_read_blocking (i2c, addr, val, 1, false);
  return val[0];
} 

// Escreve um valor de 8 bits em um registrador
void ADLX345::write8 (uint8_t reg, uint8_t val)  {  
  uint8_t aux[2];

  aux[0] = reg;
  aux[1] = val;
  i2c_write_blocking (i2c, addr, aux, 2, false);
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

  // Inicia o sensor
  ADLX345 sensor (I2C_ID, 0x53);
  sensor.begin();

  // Mostra ID do sensor
  printf ("Sensor ID: %02X\n", sensor.getId());

  // Laco Principal
  ADLX345::VECT_3D data;
  while(1) {
    sleep_ms(2000);
    sensor.getAccel(&data);
    printf ("Aceleracao X:%.1f Y:%.1f Z:%.1f\n", data.x, data.y, data.z);
  }
}
