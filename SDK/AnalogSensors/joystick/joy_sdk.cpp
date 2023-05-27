/**
 * @file joy_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Joystick  Analogico
 * @version 1.0
 * @date 2023-05-27
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Conexao dos LEDs
#define LED_RIGHT 12
#define LED_UP    13
#define LED_DOWN  14
#define LED_LEFT  15

// Conexoes do Joystick
#define PIN_HORIZ   26
#define ADC_HORIZ   0
#define PIN_VERTIC  27
#define ADC_VERTIC  1

// Configuracao do PWM
#define WRAP 4095
#define FREQ 1000.0f

// Ignorar pequenas variacoes do centro
#define ZONA_MORTA 60

// Classe para controle dos LEDs
class LED_PWM {
  private:
    int pin_num;
    uint slice_num;
    uint chan_num;
  
  public:
    // Construtor
    LED_PWM(int pin) {
      pin_num = pin;
      slice_num = pwm_gpio_to_slice_num(pin);
      chan_num = pwm_gpio_to_channel(pin);
    }

    // Inicia o pino para usar PWM
    void init(void) {
      // Configura o slice
      // f = fsys / (clock divisor * (wrap value+1)
      // clock divisor = fsys / (f * (wrap value+1))
      float fsys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS)*1000.0f;
      float div = fsys/(FREQ * (WRAP+1));
      pwm_config config = pwm_get_default_config ();
      pwm_config_set_wrap(&config, WRAP);
      pwm_config_set_clkdiv(&config, div);
      pwm_config_set_phase_correct(&config, false);
      pwm_config_set_clkdiv_mode(&config, PWM_DIV_FREE_RUNNING);
      pwm_init(slice_num, &config, false);
      pwm_set_chan_level (slice_num, chan_num, 0);
      pwm_set_enabled(slice_num, true);
      gpio_set_function(pin_num, GPIO_FUNC_PWM);
    }

    // Ajusta intensidade
    void set(uint16_t val) {
      pwm_set_chan_level (slice_num, chan_num, val);
    }
};

LED_PWM ledRight(LED_RIGHT);
LED_PWM ledLeft(LED_LEFT);
LED_PWM ledUp(LED_UP);
LED_PWM ledDown(LED_DOWN);

// Programa Principal
int main() {
    // Inicia LEDs
    ledRight.init();
    ledLeft.init();
    ledUp.init();
    ledDown.init();

    // Inicia ADC
    adc_init();
    adc_gpio_init(PIN_HORIZ);
    adc_gpio_init(PIN_VERTIC);

    // Laco Principal
    while(1) {
      // Le a posicao do joystick
      adc_select_input(ADC_HORIZ);
      adc_read();
      uint16_t horiz = adc_read(); // 0-4095
      adc_select_input(ADC_VERTIC);
      adc_read();
      uint16_t vertic = adc_read(); // 0-4095

      // Acende os LEDs conforme a posicao
      if (horiz > (2047 + ZONA_MORTA)) {
        ledRight.set(0);
        ledLeft.set(horiz-2048);
      } else if (horiz <= (2047 - ZONA_MORTA)) {
        ledRight.set(2047-horiz);
        ledLeft.set(0);
      } else {
        ledRight.set(0);
        ledLeft.set(0);
      }

      if (vertic > (2047 + ZONA_MORTA)) {
        ledDown.set(vertic-2048);
        ledUp.set(0);
      } else if (vertic <= (2047 - ZONA_MORTA)){
        ledDown.set(0);
        ledUp.set(2047-vertic);
      } else {
        ledDown.set(0);
        ledUp.set(0);
      }
    }
}
