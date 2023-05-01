// Exemplo com Sensor de Presença (PIR)

#define SENSOR_PIN 16

// Variáveis globais
int counter = 0;

// Iniciações
void setup() {
  pinMode (SENSOR_PIN, INPUT);
}

// Laço Principal
void loop() {
  Serial.print ("Sensor Disparado ");
  Serial.print (counter);
  Serial.println (" vezes");
  while (digitalRead(SENSOR_PIN) == LOW)
    ;
  while (digitalRead(SENSOR_PIN) == HIGH)
    ;
  counter++;
}
