/**
 * @file dht_sdk.c
 * @author Daniel Quadros
 * @brief Exemplo com DHT11/DHT22
 * @version 1.0
 * @date 2023-06-05
 * 
 * @copyright Copyright (c) 2022, Daniel Quadros
 * 
 */

#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Programa para o PIO:
#include "dht_sdk.pio.h"

// Conexoes dos sensores DHT (-1 se nao usado)
#define PIN_DHT11 16
#define PIN_DHT22 17

// PIO
static PIO pio = pio0;
static uint offset = 0xFFFF;

// Obtem milisegundos desde o comeco
static inline uint32_t board_millis(void)
{
	return to_ms_since_boot(get_absolute_time());
}

// Classe para acessar sensores DHT
class DHT {
    public:
        typedef enum { DHT11, DHT22 } Model;

    private:
        uint dataPin;
        Model model;
        uint sm;
        uint32_t lastreading;
        uint8_t data [5];

        // obtem dado do sensor
        bool read() {
            // Inicia e dispara a maquina de estados
            dht_program_init(pio, sm, offset, dataPin);
            // Dispara a leitura
            pio_sm_put (pio, sm, (model == DHT11) ? 969 : 54);
            // Le 5 bytes
            for (int i = 0; i < 5; i++) {
                data[i] = (uint8_t) pio_sm_get_blocking (pio, sm);
            }
            // Para a maquina de estados
            pio_sm_set_enabled (pio, sm, false);
            uint32_t total = 0;
            for (int i = 0; i < 4; i++) {
                total += data[i];
            }
            if (data[4] == (total & 0xFF)) {
                lastreading = board_millis();
                return true;
            }
            return false;
        }

        // usa dados armazenados ou faz nova leitura
        void getData() {
            // garante que temos dados
            while (lastreading == 0) {
                if (!read()) {
                    sleep_ms(2000);
                }
            }
            // usa cache se menos de 2 segundos
            uint32_t now = board_millis();
            if (lastreading > now) { // deu a volta
                lastreading = now;
            }
            if ((lastreading+2000) < now) {
                read();
            }
        }


    public:

        // Construtor
        DHT (uint pin, Model model) {
            this->dataPin = pin;
            this->model = model;
            this->sm = pio_claim_unused_sm(pio, true);
            this->lastreading = 0;
            if (offset == 0xFFFF) {
                offset = pio_add_program(pio, &dht_program);
            }
        }

        // pega umidade
        float humidity() {
            getData();
            if (model == DHT11) {
                return 0.1*data[1] + data[0];
            } else {
                return 0.1*((data[0] << 8) + data[1]);
            }
        }

        // pega temperatura
        float temperature() {
            getData();
            if (model == DHT11) {
                return 0.1*data[3] + data[2];
            } else {
                float s = (data[2] & 0x80) ? -0.1 : 0.1;
                return s*(((data[2] & 0x7f) << 8) + data[3]);
            }
        }
};


// Programa Principal
int main() {
    // Inicia stdio
    stdio_init_all();
    #ifdef LIB_PICO_STDIO_USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    #endif

    printf("\nExemplo DHT11/DHT22\n");

    DHT *dht11 = (PIN_DHT11 == -1)? NULL : new DHT(PIN_DHT11, DHT::DHT11);
    DHT *dht22 = (PIN_DHT22 == -1)? NULL : new DHT(PIN_DHT22, DHT::DHT22);

    while (true) {
        if (PIN_DHT11 != -1) {
            printf("DHT11 Umidade: %.1f%%, Temperatura: %.1fC\n",
                dht11->humidity(), dht11->temperature());
        }
        if (PIN_DHT22 != -1) {
            printf("DHT22 Umidade: %.1f%%, Temperatura: %.1fC\n",
                dht22->humidity(), dht22->temperature());
        }
        sleep_ms(3000);
    }
}
