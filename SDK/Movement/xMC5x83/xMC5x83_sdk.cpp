/**
 * @file xMC5x83_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com sensores HMC5883L/HMC5983/QMC5883L
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

const float PI = 3.14159f;

// Classe para accessar o sensor
class COMPASS {

  public:

    // Modelo do sensor
    typedef enum { UNDEFINED, HMC_58, HMC_59, QMC } MODEL;

    // Construtor
    COMPASS (i2c_inst_t *i2c, MODEL model = UNDEFINED);

    // Métodos públicos
    bool  begin(void);
    float getHeading(void);
    MODEL getModel(void);

  private:
    
    // Endereços I2C
    static const uint8_t ender_HMC = 0x1E;
    static const uint8_t ender_QMC = 0x0D;

    // Endereços dos Registradores
    static const int regCFGA_HMC = 0;
    static const int regCFGB_HMC = 1;
    static const int regMODE_HMC = 2;
    static const int regXH_HMC = 3;
    static const int regST_HMC = 9;
    
    static const int regCR1_QMC = 9;
    static const int regSR_QMC = 11;
    static const int regXL_QMC = 0;
    static const int regST_QMC = 6;

    // Instância I2C
    i2c_inst_t *i2c;

    // Endereço I2C do sensor
    uint8_t addr;

    // Modelo do sensor
    MODEL model;

    // Rotinas privativas
    bool     checkAddr(uint8_t addr);
    void     write8(uint8_t reg, uint8_t val);
    uint8_t  read8(uint8_t reg);
};

// Construtor
COMPASS::COMPASS (i2c_inst_t *i2c, MODEL model) {

  this->i2c = i2c;
  this->model = model;
}

// Configura o sensor
bool COMPASS::begin() {

  // Prepara modelo e endereço I2C
  if (model == UNDEFINED) {
    // Tenta identificar o sensor
    if (checkAddr(ender_QMC)) {
      this->addr = ender_QMC;
      this->model = QMC;
    } else if (checkAddr(ender_HMC)) {
      this->addr = ender_HMC;
      // Testa se conseguimos ligar a compensação por temperatura
      write8(regCFGA_HMC, 0x90);
      this->model = (read8(regCFGA_HMC) == 0x90)? HMC_59 : HMC_58;
    } else {
      return false; // sensor nao identificado
    }
  } else {
    this->model = model;
    this->addr = (model == QMC) ? ender_QMC: ender_HMC;
  }

  // inicia registradores para o modo continuo
  if (model == QMC) {
    write8(regSR_QMC, 0x01);  // conforme o datasheet
    write8(regCR1_QMC, 0x81); // 10Hz, 2G, 512 osr
  } else if (model == HMC_58) {
    // valores padrão, já que não encontrei um HMC5883L para testar
    write8(regCFGA_HMC, 0x10);
    write8(regCFGB_HMC, 0x01);
    write8(regMODE_HMC, 0x00);
  } else {
    // aumenta o ganho e o número de amostras
    write8(regCFGA_HMC, 0xF0);
    write8(regCFGB_HMC, 0x00);
    write8(regMODE_HMC, 0x00);
  }

  return true;
}

// Retorna o modelo do sensor
COMPASS::MODEL COMPASS::getModel() {
  return model;
}

// Obtem a direção (0 a 360 graus)
float COMPASS::getHeading() {
  uint8_t data[6];
  uint8_t reg[1];
  int16_t x,y,z;


  if (model == UNDEFINED) {
    return 0.0f;
  }

  if (model == QMC) {
    // espera os dados
    while ((read8(regST_QMC) & 0x01) == 0)
      ;
    // lê os dados
    reg[0] = regXL_QMC;
    i2c_write_blocking (i2c, addr, reg, 1, true);
    i2c_read_blocking (i2c, addr, data, 6, false);
    // converte para 16 bits
    x = data[0] + (data[1] << 8);
    y = data[2] + (data[3] << 8);
    z = data[4] + (data[5] << 8);
  } else {
    // espera os dados
    while ((read8(regST_HMC) & 0x01) == 0)
      ;
    // lê os dados
    reg[0] = regXH_HMC;
    i2c_write_blocking (i2c, addr, reg, 1, true);
    i2c_read_blocking (i2c, addr, data, 6, false);
    // converte para 16 bits
    x = data[1] + (data[0] << 8);
    z = data[3] + (data[2] << 8);
    y = data[5] + (data[4] << 8);
  }

  // calcula a direção
  float angle = atan2((float)y,(float)x);
  //printf ("%8d %8d %8d %.2f\n", x, y, z, angle/PI);
  return (angle*180.0f)/PI + 180.0f;
}

// Verifica se algum dispositivo responde ao endereço
bool COMPASS::checkAddr(uint8_t addr) {
  int ret;
  uint8_t rxdata;

  ret = i2c_read_blocking(this->i2c, addr, &rxdata, 1, false);
  return ret >= 0;
}

// Escreve um valor de 8 bits em um registrador
void COMPASS::write8 (uint8_t reg, uint8_t val) {  
  uint8_t buffer[2];

  buffer[0] = reg;
  buffer[1] = val;
  i2c_write_blocking (i2c, addr, buffer, 2, false);
}  
  
// Lê um valor de 8 bits de um registrador
uint8_t COMPASS::read8 (uint8_t reg)  {  
  uint8_t val[1];  
    
  // Seleciona o registrador
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Lê o valor
  i2c_read_blocking (i2c, addr, val, 1, false);
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

  // Inicia a interface I2C
  uint baud = i2c_init (I2C_ID, BAUD_RATE);
  printf ("I2C @ %u Hz\n", baud);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SCL_PIN);
  gpio_pull_up(I2C_SDA_PIN);

  // Inicia o sensor
  COMPASS compass (I2C_ID);
  if (!compass.begin()) {
    printf ("Sensor nao encontrado!");
    while (true) {
      sleep_ms(100);
    }
  }

  // Mostra o modelo do sensor
  COMPASS::MODEL model = compass.getModel();
  printf ("Modelo do sensor = %s\n", 
    (model == COMPASS::QMC) ? "QMC5883L" :
    (model == COMPASS::HMC_58) ? "HMC5883L" : "HMC5983"
  );

  // Laço Principal
  while(1) {
    sleep_ms(2000);
    printf ("Direcao: %.1f\n", compass.getHeading());
  }
}
