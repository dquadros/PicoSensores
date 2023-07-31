// Exemplo sensor HC-SR04

#define PIN_TRIGGER 17
#define PIN_ECHO    16

// Inciação
void setup() {
  // Init the serial
  while (!Serial) {
    delay(100);
  }
  Serial.begin(115200);

  // Configura os pinos
  pinMode (PIN_TRIGGER, OUTPUT);
  pinMode (PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIGGER, LOW);
}

// Laço Principal
void loop() {
  // Pulsa o pino Trig
  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIGGER, LOW);

  // Mede o pulso de eco
  uint32_t ellapsed = pulseIn(PIN_ECHO, HIGH, 100000);

  // Calcula a distancia
  if (ellapsed != 0) {
    float distance = (ellapsed * 0.0343) / 2.0;
    Serial.print("Distancia = ");
    Serial.print(distance, 1);
    Serial.println(" cm");
    delay(2000);
  } else {
    Serial.println("** TIMEOUT **");
  }
}
