// Exemplo com DHT11/DHT22
#include "DHT.h"

// Conexoes dos sensores DHT (-1 se nao usado)
#define PIN_DHT11 16
#define PIN_DHT22 17

#if PIN_DHT11 != -1
DHT dht11(PIN_DHT11, DHT11);
#endif

#if PIN_DHT22 != -1
DHT dht22(PIN_DHT22, DHT22);
#endif

// Iniciacao
void setup() {
  Serial.begin(115200);
  #if PIN_DHT11 != -1
  dht11.begin();
  #endif
  #if PIN_DHT22 != -1
  dht22.begin();
  #endif
}

// Laco Principal
void loop() {
  #if PIN_DHT11 != -1
  Serial.print("DHT11  Umidade: ");
  Serial.print(dht11.readHumidity(), 1);
  Serial.print("%, Temperatura: ");
  Serial.print(dht11.readTemperature(), 1);
  Serial.println("C");
  #endif

  #if PIN_DHT22 != -1
  Serial.print("DHT22  Umidade: ");
  Serial.print(dht22.readHumidity(), 1);
  Serial.print("%, Temperatura: ");
  Serial.print(dht22.readTemperature(), 1);
  Serial.println("C");
  #endif

  delay(3000);
}
