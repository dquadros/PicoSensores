// Exemplo com Sensor de Chama

#define BUZZER_PIN 15
#define SENSOR_PIN 16

// Iniciações
void setup() {
  pinMode (BUZZER_PIN, OUTPUT);
  digitalWrite (BUZZER_PIN, LOW);
  pinMode (SENSOR_PIN, INPUT);
}

// Laço Principal
void loop() {
  if (digitalRead(SENSOR_PIN) == LOW) {
    digitalWrite (BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite (BUZZER_PIN, LOW);
  }
}
