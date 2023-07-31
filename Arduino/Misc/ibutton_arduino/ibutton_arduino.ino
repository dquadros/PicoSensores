// exemplo com iButton
// Requires the OneWireNg library

#include "OneWireNg_CurrentPlatform.h"
#include "utils/Placeholder.h"


// Pico ligado a rede 1-wire
# define OW_PIN         16

static Placeholder<OneWireNg_CurrentPlatform> ow;

static OneWireNg::Id last;

// Iniciação
void setup() {
  // Inicia a serial
  while (!Serial) {
    delay(100);
  }
  Serial.begin(115200);

  // Inicia a rede 1-wire
  new (&ow) OneWireNg_CurrentPlatform(OW_PIN, false);
  memset(last, 0xFF, sizeof(OneWireNg::Id));
}

// Laco Principal
void loop() {
  OneWireNg::Id id;

  ow->searchReset();
  if ((ow->search(id) == OneWireNg::EC_SUCCESS) &&
      (id[0] == 0x01) && 
      (memcmp(id, last, sizeof(OneWireNg::Id)) != 0)) {
    Serial.print("ID: ");
    for (size_t i = sizeof(OneWireNg::Id)-2; i > 0; i--) {
        Serial.print(id[i], HEX);
    }
    Serial.println();
    memcpy(last, id, sizeof(OneWireNg::Id));
    delay(2000);
  } else {
    memset(last, 0xFF, sizeof(OneWireNg::Id));
    delay (500);
  }
}
