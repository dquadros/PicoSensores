/**
 * @file keypad_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Exemplo com Teclado
 * @version 1.0
 * @date 2023-04-19
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"


// Pinos do Teclado
#define NROWS     4
#define NCOLUMNS  3
#define ROW_1     0
#define ROW_2     1
#define ROW_3     2
#define ROW_4     3
#define COL_1     4
#define COL_2     5
#define COL_3     6

#define MSK_ROWS ((1 << ROW_1) | (1 << ROW_2) | (1 << ROW_3) | (1 << ROW_4))
#define MSK_COLS ((1 << COL_1) | (1 << COL_2) | (1 << COL_3))

// Debounce
#define DEBOUNCE 5

// Variaveis de controle do teclado
int rows[] = { ROW_1, ROW_2, ROW_3, ROW_4 };
int columns[] = { COL_1, COL_2, COL_3 };
int curRow;
uint32_t previous [NROWS];
uint32_t validated [NROWS];
int count [NROWS][NCOLUMNS];

// Codigo das teclas
char codes[NROWS][NCOLUMNS] = { 
 { '1', '2', '3' },
 { '4', '5', '6' },
 { '7', '8', '9' },
 { '*', '0', '#' }
};

// Fila do teclado
#define MAX_KEYS 16
char keys[MAX_KEYS+1];
int inkey, outkey;

// Timer para varrer o teclado
static struct repeating_timer timer;

// Verifica teclas apertadas na linha atual
bool checkRow(struct repeating_timer *t) {
  // coloca a linha atual no nivel BAIXO
  int pin = rows[curRow];
  gpio_set_dir (pin, true);
  gpio_put(pin, false);

  // le as colunas e verifica mudancas
  for (int col = 0; col < NCOLUMNS; col++) {
    uint32_t msk = 1u << columns[col];
    uint32_t read = gpio_get_all() & msk;
    if (read == (previous[curRow] & msk)) {
      if (count[curRow][col] != 0) {
        if (--count[curRow][col] == 0) {
          // leitura confirmada
          if (read != (validated[curRow] & msk)) {
            validated[curRow] = (validated[curRow] & ~msk) | read;
            if (read == 0) {
              // detectada tecla apertada
              int aux = inkey+1;
              if (aux > MAX_KEYS) {
                aux = 0;
              }
              if (aux != outkey) {
                keys[inkey] = codes[curRow][col];
                inkey = aux;
              }
            }
          }
        }
      }
    } else {
      // reinicia validacao
      previous[curRow] = (previous[curRow] & ~msk) | read;
      count[curRow][col] = DEBOUNCE;
    }
  }
  // reconfigura linha como entrada
  gpio_set_dir (pin, false);
  // passa para a proxima linha
  curRow = (curRow+1) % NROWS;

  return true; // continuar executando
}

// Le uma tecla da fila, retorna -1 se fila vazia
static int getKey(void) {
    int key = -1;
    if (inkey != outkey) {
        key = keys[outkey];
        outkey = (outkey == MAX_KEYS) ? 0 : (outkey + 1);
    }
    return key;
}

// Programa principal
int main() {
  // Inicia stdio
  stdio_init_all();
  #ifdef LIB_PICO_STDIO_USB
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  #endif

  printf("\nExemplo de teclado\n");

  // init keypad controls
  curRow = 0;
  for (int i = 0; i < NROWS; i++) {
    previous[i] = validated[i] = 0;
    for (int j = 0; j < NCOLUMNS; j++) {
      count[i][j] = DEBOUNCE;
    }
  }

  // inicia fila do teclado
  inkey = outkey = 0;

  // inicia pinos do teclado
  gpio_init_mask (MSK_ROWS | MSK_COLS);
  for (int i = 0; i < NCOLUMNS; i++) {
    gpio_set_pulls(columns[i], true, false);
  }

  // varrer teclado a cada 10 milissegundos
  add_repeating_timer_ms(10, checkRow, NULL, &timer);

  // Laco principal
  while(1) {
    int key = getKey();
    if (key == -1) {
      sleep_ms(100);
    } else {
      printf ("Apertou %c\n", key);
    }
  }
}