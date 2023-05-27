// Exemplo com Fototransistor

// Pinos
#define SENSOR_PIN A0

// iniciacao
void setup() {
  Serial.begin(115200);
}

// Laco Principal
void loop() {
  static uint16_t val_ant = 0;
  uint16_t val = analogRead(SENSOR_PIN);
  int dif = (val > val_ant) ? val - val_ant : val_ant - val;
  if (dif > 100) {
    Serial.println(val);
    val_ant = val;
  }
}
