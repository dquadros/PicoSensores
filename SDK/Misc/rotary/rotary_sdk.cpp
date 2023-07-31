/**
 * @file rotary_sdk.c
 * @author Daniel Quadros
 * @brief Exemplo com Rotary Encoder
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
#include "encoder.pio.h"

//  Conexões do encoder
#define ENCODER_A_PIN 16
#define ENCODER_B_PIN 17


// Informaçõo de estado do programa PIO
static const uint32_t STATE_A_MASK      = 0x80000000;
static const uint32_t STATE_B_MASK      = 0x40000000;
static const uint32_t STATE_A_LAST_MASK = 0x20000000;
static const uint32_t STATE_B_LAST_MASK = 0x10000000;

static const uint32_t STATES_MASK = STATE_A_MASK | STATE_B_MASK |
                                    STATE_A_LAST_MASK | STATE_B_LAST_MASK;

#define LAST_STATE(state)  ((state) & 0b0011)
#define CURR_STATE(state)  (((state) & 0b1100) >> 2)

enum MicroStep : uint8_t {
    MICROSTEP_0 = 0b00,
    MICROSTEP_1 = 0b10,
    MICROSTEP_2 = 0b11,
    MICROSTEP_3 = 0b01,
};

static uint8_t move_up = (MICROSTEP_2 << 6) | (MICROSTEP_3 << 4) | (MICROSTEP_0 << 2) | MICROSTEP_1;
static uint8_t move_down = (MICROSTEP_1 << 6) | (MICROSTEP_0 << 4) | (MICROSTEP_3 << 2) | MICROSTEP_2;

// PIO
static PIO enc_pio = pio0;
static uint enc_sm;


// Prepara o programa PIO
void encoder_program_init(uint enc_pin_a, uint enc_pin_b) {
  // Carrega o programa e seleciona a máquina de estados
  uint offset = pio_add_program(enc_pio, &encoder_program);
  enc_sm = pio_claim_unused_sm(enc_pio, true);

  // Configura os pinos
  pio_gpio_init(enc_pio, enc_pin_a);
  pio_gpio_init(enc_pio, enc_pin_b);
  gpio_pull_up(enc_pin_a);
  gpio_pull_up(enc_pin_b);
  pio_sm_set_consecutive_pindirs(enc_pio, enc_sm, enc_pin_a, 1, false);
  pio_sm_set_consecutive_pindirs(enc_pio, enc_sm, enc_pin_b, 1, false);

  // Configura a máquina de estados
  pio_sm_config c = encoder_program_get_default_config(offset);
  sm_config_set_jmp_pin(&c, enc_pin_a);
  sm_config_set_in_pins(&c, enc_pin_b);
  sm_config_set_in_shift(&c, false, false, 1);
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
  sm_config_set_clkdiv_int_frac(&c, 250, 0);
  pio_sm_init(enc_pio, enc_sm, offset, &c);

  // Inicia o estado
  bool enc_state_a = gpio_get(enc_pin_a);
  bool enc_state_b = gpio_get(enc_pin_b);
  pio_sm_exec(enc_pio, enc_sm, pio_encode_set(pio_x, (uint)enc_state_a << 1 | (uint)enc_state_b));

  // inicia a execução
  pio_sm_set_enabled(enc_pio, enc_sm, true);    
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

    printf("\nExemplo com Rotary Encoder\n");

    // Inicia e dispara a máquina de estados
    encoder_program_init(ENCODER_A_PIN, ENCODER_B_PIN);

    uint8_t history = 0;
    int val = 50;

    printf ("%3d", val);

    while (true) {
        // espera mover
        uint32_t received = pio_sm_get_blocking(enc_pio, enc_sm);

        // extrai novo estado e coloca no histórico
        uint8_t states = (received & STATES_MASK) >> 28;
        history = ((history & 0x3F) << 2) | CURR_STATE(states);

        // teste padrões de movimento para frente e para trás
        if (history == move_up) {
            if (val < 100) {
                val++;
                printf ("\r%3d", val);
            }
        }
        else if (history == move_down) {
            if (val > 0) {
                val--;
                printf ("\r%3d", val);
            }
        }
    }
}
