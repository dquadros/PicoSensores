/**
 * @file button_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Botao
 * @version 1.0
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


static inline uint32_t board_millis(void)
{
	return to_ms_since_boot(get_absolute_time());
}

class Button {
  private:
    int pinButton;
    bool pressed;
    int debounce;
    bool last;
    uint32_t lastTime;

  public:

    // Construtor
    Button (int pin, int debounce = 20) {
      pinButton = pin;
      gpio_init(pinButton);
      gpio_set_pulls(pinButton, true, false);
      pressed = false;
      this->debounce = debounce;
      last = gpio_get(pinButton) == 0;
      lastTime = board_millis();
    }

  // Testa se botao apertado
  bool isPressed() {
    bool val = gpio_get(pinButton) == 0;
    if (val != last) {
      // leitura mudou
      last = val;
      lastTime = board_millis();
    } else if (val != pressed) {
      int dif = board_millis() - lastTime;
      if (dif > debounce) {
        // atualiza estado do botao
        pressed = val;
      }
    }
    return pressed;
  }
};

// Programa principal
int main() {
    // Inicia stdio
    stdio_init_all();
    #ifdef LIB_PICO_STDIO_USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    #endif

    printf("\nExemplo com Botao\n");

    Button button(15);
    int counter = 0;

    while(1) {
        printf ("Botao apertado %d vezes\n", counter);
        while (button.isPressed())
            ;
        while (!button.isPressed())
            ;
        counter++;
    }
}