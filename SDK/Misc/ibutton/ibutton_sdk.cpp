/**
 * @file ibutton_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com iButton
 * @version 1.0
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico-onewire/api/one_wire.h"

// Pinos
#define SENSOR_PIN 16

static One_wire one_wire(SENSOR_PIN);

#define  FAMILY_CODE_DS1990A 0x01

static rom_address_t sensor;

// Programa Principal
int main() {
  // Inicia stdio
  stdio_init_all();
  #ifdef LIB_PICO_STDIO_USB
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  #endif

  // Inicia rede OneWire
  one_wire.init();

  // Laço principal
  memset (sensor.rom, 0xFF, sizeof(sensor.rom));
  while(1) {
  
    // Testa se tem um sensor
    int count = one_wire.find_and_count_devices_on_bus();
    if (count > 0) {
      // Achou um sensor
      auto address = One_wire::get_address(0);
      if ((address.rom[0] == FAMILY_CODE_DS1990A) &&
        (memcmp (address.rom, sensor.rom, sizeof(sensor.rom) != 0))) {
          memcpy (&sensor, &address, sizeof(sensor));
        printf("ID: %02X%02X%02X%02X%02X%02X\n",
            address.rom[6], address.rom[5], address.rom[4], 
            address.rom[3], address.rom[2], address.rom[1]);
        sleep_ms(2000);
      }
    } else {
      memset (sensor.rom, 0xFF, sizeof(sensor.rom));
      sleep_ms(500);
    }
  }
}
