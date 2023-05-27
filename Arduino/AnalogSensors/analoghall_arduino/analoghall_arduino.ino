// Exemplo com Sensor Efeito Hall Analogico

// Pin0s
#define SENSOR_PIN A0

//  Leitura sem campo magnetico
int zero;

// Le o sensor (media de 50 leituras)
#define N_READINGS 50
uint readSensor() {
  uint32_t sum = 0;
  for (int i = 0; i < N_READINGS; i++) {
    sum += analogRead(SENSOR_PIN);
  }
  return sum / N_READINGS;
}

// Iniciacao
void setup() {
  Serial.begin(115200);

  // Assume que nao tem campo magnetico no inicio
  zero = readSensor() ;
  Serial.print("Zero = ");
  Serial.println(zero);
  delay(1000);
  Serial.println ("Pronto");
}

// Main loop
void loop() {
  int field = readSensor();
  if (((field-zero) < 10) && ((field-zero) > -10)) {
      Serial.println ("Sem campo");
  } else if (field > zero) {
    Serial.print ("Campo = S ");
    Serial.println(field-zero);
  } else {
    Serial.print ("Campo = N ");
    Serial.println(zero-field);
  }
  delay(1000);
}
