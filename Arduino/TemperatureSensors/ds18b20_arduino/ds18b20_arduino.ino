//Exemplo Sensor de Temperatura DS18B20
// Requer a biblioteca OneWireNg
// Adaptado do exemplo DallasTemperature

#include "OneWireNg_CurrentPlatform.h"
#include "drivers/DSTherm.h"
#include "utils/Placeholder.h"

// via One Wire conectada a este pino
# define OW_PIN         16

static Placeholder<OneWireNg_CurrentPlatform> ow;

// Iniciacao
void setup() {
  // Inicia a serial
  Serial.begin(115200);

  // Instancia a via onewire
  new (&ow) OneWireNg_CurrentPlatform(OW_PIN, false);
}

// Laco principal
void loop() {
  DSTherm drv(ow);

  // Dispara a conversao em todos os sensores
  drv.convertTempAll(DSTherm::MAX_CONV_TIME, false);

  // Le a temperatura dos sensores
  Placeholder<DSTherm::Scratchpad> scrpd;
  for (const auto& id: *ow) {
      if (printId(id)) {
        if (drv.readScratchpad(id, scrpd) == OneWireNg::EC_SUCCESS) {
          const uint8_t *scrpd_raw = ((const DSTherm::Scratchpad&) scrpd).getRaw();
          long temp = ((const DSTherm::Scratchpad&) scrpd).getTemp();
          Serial.print(" Temp:");
          if (temp < 0) {
              temp = -temp;
              Serial.print('-');
          }
          Serial.print(temp / 1000);
          Serial.print('.');
          Serial.print(temp % 1000);
          Serial.print(" C");
        }
        Serial.println();
      }
  }
  Serial.println();
  delay(250);
}

// Mostra o endereco e a familia do dispositivo
// retorna falseo se nao suportado
static bool printId(const OneWireNg::Id& id)
{
    const char *name = DSTherm::getFamilyName(id);

    Serial.print(id[0], HEX);
    for (size_t i = 1; i < sizeof(OneWireNg::Id); i++) {
        Serial.print(':');
        Serial.print(id[i], HEX);
    }
    if (name) {
        Serial.print(" -> ");
        Serial.print(name);
    }

    return (name != NULL);
}

