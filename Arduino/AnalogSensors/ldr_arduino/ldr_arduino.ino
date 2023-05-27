// Exemplo com LDR

// Pinos
#define BUZZER_PIN 15
#define SENSOR_PIN A0

// Limite claro/escuro
#define DARK 75

// inciacao
void setup() {
  pinMode (BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

// Laco principal
void loop() {
  // Dorme por 3 a 60 segundos
  uint tsleep = (rand() % 57000) + 3000;
  delay (tsleep);
  // Bipe curto se escuro
  uint16_t val = analogRead(SENSOR_PIN); // 0-1023
  if (val < DARK) {
    digitalWrite (BUZZER_PIN, HIGH);
    delay (100);
    digitalWrite(BUZZER_PIN, LOW);
  }
}
