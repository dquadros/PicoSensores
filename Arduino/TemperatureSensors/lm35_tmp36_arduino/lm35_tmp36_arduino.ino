// Exemplo com os Sensores de Temperatura LM35D e TMP36

// Pinos (-1 se )
#define LM35_PIN A0
#define TMP36_PIN A1

// retorna a tensão em pino ADC
// faz a média de 10 leituras
#define N_READINGS 10
float readSensor(int pin) {
  uint32_t sum = 0;

  if (pin == -1) {
    return 0;
  }

  for (int i = 0; i < N_READINGS; i++) {
    sum += analogRead(pin);
  }
  return (sum*3.3 / N_READINGS)/1024.0;
}

// Iniciação
void setup() {
  Serial.begin(115200);
}

// Laço Principal
void loop() {
  float tempLM35 = readSensor(LM35_PIN)/0.01;
  float tempTMP36 = 25.0 + (readSensor(TMP36_PIN)-0.75)/0.01;
  Serial.print("LM35 = ");
  Serial.print(tempLM35,1);
  Serial.print("C  TMP36 = ");
  Serial.print(tempTMP36,1);
  Serial.println("C");
  delay(2000);
}