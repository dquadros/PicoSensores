// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

#define ENC_DEBOUNCE_CYCLES 500

// ------- //
// encoder //
// ------- //

#define encoder_wrap_target 0
#define encoder_wrap 10

static const uint16_t encoder_program_instructions[] = {
            //     .wrap_target
    0xa047, //  0: mov    y, osr                     
    0x0082, //  1: jmp    y--, 2                     
    0xa0e2, //  2: mov    osr, y                     
    0x00c6, //  3: jmp    pin, 6                     
    0xa0c3, //  4: mov    isr, null                  
    0x0008, //  5: jmp    8                          
    0xe041, //  6: set    y, 1                       
    0xa0c2, //  7: mov    isr, y                     
    0x4001, //  8: in     pins, 1                    
    0xa046, //  9: mov    y, isr                     
    0x01ab, // 10: jmp    x != y, 11             [1] 
            //     .wrap
    0x4022, // 11: in     x, 2                       
    0xa02f, // 12: mov    x, !osr                    
    0x403c, // 13: in     x, 28                      
    0x8000, // 14: push   noblock                    
    0xa022, // 15: mov    x, y                       
    0xe95d, // 16: set    y, 29                  [9] 
    0x0f91, // 17: jmp    y--, 17                [15]
    0xa04b, // 18: mov    y, !null                   
    0x0094, // 19: jmp    y--, 20                    
    0xa0e2, // 20: mov    osr, y                     
    0x0100, // 21: jmp    0                      [1] 
};

#if !PICO_NO_HARDWARE
static const struct pio_program encoder_program = {
    .instructions = encoder_program_instructions,
    .length = 22,
    .origin = -1,
};

static inline pio_sm_config encoder_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + encoder_wrap_target, offset + encoder_wrap);
    return c;
}

static const uint8_t ENC_LOOP_CYCLES = encoder_wrap - encoder_wrap_target;
// The time that the debounce takes, as the number of wrap loops that the debounce is equivalent to
static const uint8_t ENC_DEBOUNCE_TIME = ENC_DEBOUNCE_CYCLES / ENC_LOOP_CYCLES;

#endif
