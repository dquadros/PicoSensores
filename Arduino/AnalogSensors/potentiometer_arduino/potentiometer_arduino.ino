// Exemplo com Potenciometro

// Pinos
#define PIN_SENSOR A0
#define PIN_LED    25

// Initiaciacao
void setup() {
  Serial.begin(115200);
}

// Laco Principal
void loop() {
  int val = analogRead(PIN_SENSOR);
  Serial.println(val);
  // Converte val de 0-1023 para 0-256
  analogWrite(PIN_LED, val>>2);
  delay(200);
}
