/**
 * @file loadcell_sdk.c
 * @author Daniel Quadros
 * @brief Exemplo com Célula de Carga
 * @version 1.0
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include "stdio.h"
#include "string.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "lcd_i2c.h"

// Programa da PIO
#include "hx711.pio.h"

// Conexões do sensor
#define PIN_DATA    12
#define PIN_CLOCK   13

// Conexões do display
#define PIN_SDA 16
#define PIN_SCL 17

// Conexão do botão
#define PIN_BUTTON 15

// PIO
static PIO pio = pio0;
static uint offset;
static uint sm;

// Parametro para converter valor lido em kg
float tare, scale;

// Rotinas locais
void keyPress(const char *msg);
void calibrate(void);
float hx711_avg(int count);

// Programa Principla
int main() {
    // Inicia e dispara a máquina de estados
    sm = pio_claim_unused_sm(pio, true);
    offset = pio_add_program(pio, &hx711_program);
    hx711_program_init(pio, sm, offset, PIN_DATA, PIN_CLOCK);

    // Inicia o display
    lcd_init (i2c0, PIN_SDA, PIN_SCL, 0x27);
    lcd_backlight(true);

    // Inicia o botão
    gpio_init(PIN_BUTTON);
    gpio_set_pulls(PIN_BUTTON, true, false);

    // Calibra o sensor
    calibrate();
    lcd_write(0, 0, "Balanca pronta");

    // Laço Principal
    while (true) {
        char buf[20];
        float weight = (hx711_avg(10) - tare)/scale;
        sprintf(buf,"%7.3f", weight);
        strcat(buf,"kg");
        lcd_write(1, 0, buf);
        sleep_ms(500);
    }
}

// Obtem uma leitura média
float hx711_avg(int count) {
    int32_t sum = 0;
    pio_sm_clear_fifos (pio, sm);
    for (int i = 0; i < count; i++) {
        int32_t val = pio_sm_get_blocking(pio, sm);
        if (val & 0x800000) {
            val = val - 0x1000000;
        }
        sum += val;
    }
    return ((float)sum)/count;
}

// Calibração inicial
void calibrate() {
    keyPress("Balanca vazia");
    tare = hx711_avg(50);
    keyPress("Coloque 1kg");
    scale = (hx711_avg(50) - tare);
}

// Mostra mensagem e espera botão
void keyPress(const char *msg) {
    lcd_clear();
    lcd_write(0,0,msg);
    lcd_write(1,0,"Aperte o botao");
    while (gpio_get(PIN_BUTTON) == 1) {
        sleep_ms(100);
    }
    sleep_ms(100); // debounce
    lcd_write(1,0,"Solte o botao ");
    while (gpio_get(PIN_BUTTON) == 0) {
        sleep_ms(100);
    }
    sleep_ms(100); // debounce
    lcd_write(1,0,"Aguarde       ");
}
