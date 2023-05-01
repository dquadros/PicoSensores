// Exemplo com Sensor de Som

#define SENSOR_PIN 16

// Iniciações
void setup() {
  pinMode (LED_BUILTIN, OUTPUT);
  digitalWrite (LED_BUILTIN, LOW);
  pinMode (SENSOR_PIN, INPUT);
}

// Laço Principal
void loop() {
  if (digitalRead(SENSOR_PIN) == HIGH) {
    digitalWrite (LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite (LED_BUILTIN, LOW);
  }
}
