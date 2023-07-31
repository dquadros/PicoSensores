/**
 * @file bmp390_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com o Sensor BMP390
 * @version 1.0
 * @date 2023-06-23
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "bmp3.h"

// Sensor connections
#define I2C_ID        i2c0
#define I2C_SCL_PIN   17
#define I2C_SDA_PIN   16

#define BAUD_RATE     1000000   // 1MHz

#define BMP_ADDR 0x77

// Variaveis Globais
static struct bmp3_dev dev;
static uint8_t dev_addr;

// Rotinas Locais
static void initSensor(void);
static BMP3_INTF_RET_TYPE bmp3_interface_init(struct bmp3_dev *bmp3);
static void bmp3_check_rslt(const char api_name[], int8_t rslt);

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
  initSensor();
  printf ("Pronto\n");
  
  // Laco Principal
  struct bmp3_data data = { 0 };
  struct bmp3_status status = { { 0 } };
  while(1) {
    int8_t rslt = bmp3_get_status(&status, &dev);
    if ((rslt == BMP3_OK) && (status.intr.drdy == BMP3_ENABLE)) {
      rslt = bmp3_get_sensor_data(BMP3_PRESS_TEMP, &data, &dev);
      bmp3_check_rslt("bmp3_get_sensor_data", rslt);
      bmp3_get_status(&status, &dev); // limpa drdy
      printf("Temperatura: %.2f C  Pressao: %.0f Pa\n",
          data.temperature, data.pressure);
    }
  }
}

// Inicia o sensor
static void initSensor(void) {
    int8_t rslt;
    uint16_t settings_sel;
        struct bmp3_settings settings = { 0 };
    struct bmp3_status status = { { 0 } };

    rslt = bmp3_interface_init(&dev);
    bmp3_check_rslt("bmp3_interface_init", rslt);

    rslt = bmp3_init(&dev);
    bmp3_check_rslt("bmp3_init", rslt);

    settings.int_settings.drdy_en = BMP3_ENABLE;
    settings.press_en = BMP3_ENABLE;
    settings.temp_en = BMP3_ENABLE;

    settings.odr_filter.press_os = BMP3_OVERSAMPLING_2X;
    settings.odr_filter.temp_os = BMP3_OVERSAMPLING_2X;
    settings.odr_filter.odr = BMP3_ODR_0_2_HZ;

    settings_sel = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN | BMP3_SEL_PRESS_OS |
                   BMP3_SEL_TEMP_OS | BMP3_SEL_ODR | BMP3_SEL_DRDY_EN;

    rslt = bmp3_set_sensor_settings(settings_sel, &settings, &dev);
    bmp3_check_rslt("bmp3_set_sensor_settings", rslt);

    settings.op_mode = BMP3_MODE_NORMAL;
    rslt = bmp3_set_op_mode(&settings, &dev);
    bmp3_check_rslt("bmp3_set_op_mode", rslt);
}

// Verifica erros
static void bmp3_check_rslt(const char api_name[], int8_t rslt) {
  switch (rslt) {
    case BMP3_OK:
      // linha abaixo para debug
      //printf ("API:%s OK (%d)\n", api_name, rslt);
      break;
    case BMP3_E_NULL_PTR:
      printf ("API:%s Error:Null pointer (%d)\n", api_name, rslt);
      break;
    case BMP3_E_COMM_FAIL:
      printf ("API:%s Error:Communication failure (%d)\n", api_name, rslt);
      break;
    case BMP3_E_INVALID_LEN:
      printf ("API:%s Error:Incorrect length (%d)\n", api_name, rslt);
      break;
    case BMP3_E_DEV_NOT_FOUND:
      printf ("API:%s Error:Sensor not found (%d)\n", api_name, rslt);
      break;
    case BMP3_E_CONFIGURATION_ERR:
      printf ("API:%s Error:Configuration erro (%d)\n", api_name, rslt);
      break;
    case BMP3_W_SENSOR_NOT_ENABLED:
      printf ("API:%s Warning:Sensor not enabled (%d)\n", api_name, rslt);
      break;
    case BMP3_W_INVALID_FIFO_REQ_FRAME_CNT:
      printf ("API:%s Warning:Invalid watermark level (%d)\n", api_name, rslt);
      break;
    default:
      printf ("API:%s Error:Unknown (%d)\n", api_name, rslt);
      break;
  }
}


// Rotinas de Interface para a API Bosch

/*!
 * Delay
 */
static void bmp3_user_delay_us(uint32_t period, void *intf_ptr) {
  sleep_us(period);
}

/*!
 * Leitura de I2C
 */
static BMP3_INTF_RET_TYPE bmp3_user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
  // Seleciona o registrador
  i2c_write_blocking (I2C_ID, BMP_ADDR, &reg_addr, 1, true);

  // Le os dados
   i2c_read_blocking (I2C_ID, BMP_ADDR, reg_data, len, false);

  return BMP3_OK;
}

/*!
 * Escrita no I2C
 */
static BMP3_INTF_RET_TYPE bmp3_user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
  // O SDK nao tem uma função para fazer escrita continua de dois buffers
  // precisamos concatenar "na unha" reg_addr e reg_data
  uint8_t *aux = malloc (len+1);
  aux[0] = reg_addr;
  memcpy(aux+1, reg_data, len);
  i2c_write_blocking (I2C_ID, BMP_ADDR, aux, len+1, false);
  free (aux);

  return BMP3_OK;
}

// Inicia interface
static BMP3_INTF_RET_TYPE bmp3_interface_init(struct bmp3_dev *bmp3) {
  int8_t rslt = BMP3_OK;

  if (bmp3 != NULL) {
    dev_addr = BMP_ADDR;
    bmp3->intf = BMP3_I2C_INTF;
    bmp3->read = bmp3_user_i2c_read;
    bmp3->write = bmp3_user_i2c_write;
    bmp3->delay_us = bmp3_user_delay_us;
    bmp3->intf_ptr = &dev_addr;
  } else {
    rslt = BMP3_E_NULL_PTR;
  }
  
  return rslt;
}
