/**
 * @file mma8452_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com sensor MMA8452
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

// Classe para accessar o sensor
class MMA8452 {

  public:

    typedef struct {
      float x;
      float y;
      float z;
    } VECT_3D;

    // Endereço I2C padrão
    static const uint8_t I2C_ADDR = 0x1C;

    // ID do dispositivo
    static const uint8_t ID = 42;

    // Construtor
    MMA8452 (i2c_inst_t *i2c, uint8_t addr = MMA8452::I2C_ADDR);

    // Métodos públicos
    void    begin(void);
    uint8_t getId(void);
    void    getAccel(VECT_3D *vect, float scale = 1024.0);
    void    active(void);
    void    standby(void);

  private:
    
    // Endereços dos registradores
    static const uint8_t XYZ_DATA_CFG = 0x0E;
    static const uint8_t WHO_AM_I = 0x0D;
    static const uint8_t CTRL_REG1 = 0x2A;
    static const uint8_t PULSE_CFG = 0x21;
    static const uint8_t OUT_X_MSB = 0x01;

    // Instância do I2C
    i2c_inst_t *i2c;

    // Endereço I2C do sensor
    uint8_t addr;

    // ID do sensor
    uint8_t id;

    // Dado bruto
    int16_t raw[14];

    // Rotinas privativas
    void     readRaw(void);
    uint8_t  read8(uint8_t reg);
    void     write8(uint8_t reg, uint8_t val);
};

// Construtor
MMA8452::MMA8452 (i2c_inst_t *i2c, uint8_t addr) {
  this->i2c = i2c;
  this->addr = addr;
}

// Iniciação do sensor
void MMA8452::begin() {
  this->id = read8(WHO_AM_I);
  standby();
  uint8_t val = read8(XYZ_DATA_CFG);
  write8(XYZ_DATA_CFG, val& 0xFC);  // escala = 2g
  val = read8(CTRL_REG1);
  write8(CTRL_REG1, val & 0xC7);  // odr = 500Hz
  write8(PULSE_CFG, 0x00);  // desliga detecção de batida
  active();
}

// Retorna o id do sensor
uint8_t MMA8452::getId() {
  return id;
}

// Entra no modo ativo
void MMA8452::active() {
  uint8_t val = read8(CTRL_REG1);
  write8(CTRL_REG1, val | 0x01);
}

// Entra no modo standby
void MMA8452::standby() {
  uint8_t val = read8(CTRL_REG1);
  write8(CTRL_REG1, val & 0xFE);
}


// Lê a aceleração em g
void MMA8452::getAccel(VECT_3D *vect, float scale) {
  readRaw();
  vect->x = (float) raw[0] / scale;
  vect->y = (float) raw[1] / scale;
  vect->z = (float) raw[2] / scale;
}

// Lê valores brutos
void MMA8452::readRaw() {
  uint8_t data[6];

  // Seleciona o primeiro registrador
  uint8_t reg = OUT_X_MSB;
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Lê os valores
  i2c_read_blocking (i2c, addr, data, 6, false);

  // Converte em int16
  for (int i = 0; i < 3; i++) {
    raw[i] = (data[2*i] << 8) | data[2*i+1];
    raw[i] /= 16;
  }
}

// Le valor de 8 bits de um registrador
uint8_t MMA8452::read8 (uint8_t reg)  {  
  uint8_t val[1];  
    
  // Seleciona o register
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Lê o valor
  i2c_read_blocking (i2c, addr, val, 1, false);
  return val[0];
} 

// Escreve valor de 8 bits em um registrador
void MMA8452::write8 (uint8_t reg, uint8_t val)  {  
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

  // Inicia sensor
  MMA8452 sensor (I2C_ID);
  sensor.begin();

  // Mostra ID do sensor
  printf ("ID do sensor: %02X\n", sensor.getId());

  // Laço principal
  MMA8452::VECT_3D data;
  while(1) {
    sleep_ms(2000);
    sensor.getAccel(&data);
    printf ("Aceleracao X:%.1f Y:%.1f Z:%.1f\n", data.x, data.y, data.z);
  }
}
