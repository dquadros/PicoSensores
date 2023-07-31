/**
 * @file hcsr04_sdk.c
 * @author Daniel Quadros
 * @brief Exemplo com sensorHC-SR04
 * @version 1.0
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Programa da PIO
#include "hcsr04.pio.h"

// Conexões do sensor
#define PIN_TRIGGER 17
#define PIN_ECHO    16

// PIO
static PIO pio = pio0;
static uint offset;
static uint sm;

#define TIMEOUT 300000

int main() {
    // Inicia stdio
    stdio_init_all();
    #ifdef LIB_PICO_STDIO_USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    #endif

    printf("\nExemplo com HC-SR04\n");

    // Inicia e dispara a máquina de estado
    sm = pio_claim_unused_sm(pio, true);
    offset = pio_add_program(pio, &hcsr04_program);
    hcsr04_program_init(pio, sm, offset, PIN_TRIGGER, PIN_ECHO);

    // Laco principal
    while (true) {
        pio_sm_put (pio, sm, TIMEOUT);
        uint32_t val = pio_sm_get_blocking (pio, sm);
        if (val < TIMEOUT) {
            uint32_t ellapsed = TIMEOUT - val;
            float distance = (ellapsed * 0.0343) / 2;
            printf ("Distancia = %.1f cm\n", distance);
            sleep_ms(2000);
        } else {
            printf ("** TIMEOUT **\n");
        }
    }
}
