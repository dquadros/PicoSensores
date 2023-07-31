// Exemplo com o sensor MPU6050

#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 sensor(Wire);
long nextReading = 0L;

// Iniciação
void setup() {
  while(!Serial) {
    delay(100);
  }
  Serial.begin(115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();
  sensor.begin();

  // Determina os valores com o sensor em repouso horizontal
  Serial.println("Calculando os offsets, nao mova o MPU6050");
  delay(1000);
  sensor.calcOffsets(true,true);
  Serial.println("Pronto!\n");
}

// Laco Principal
void loop() {
  // charmar update() frequentemente para obter angulos
  sensor.update();
  if (millis() > nextReading) {
    Serial.print ("Aceleracao X:");
    Serial.print (sensor.getAccX(), 1);
    Serial.print ("g Y:");
    Serial.print (sensor.getAccY(), 1);
    Serial.print ("g Z:");
    Serial.print (sensor.getAccZ(), 1);
    Serial.println ("g");

    Serial.print ("Giroscopio X:");
    Serial.print (sensor.getGyroX(), 1);
    Serial.print ("dg/s Y:");
    Serial.print (sensor.getGyroY(), 1);
    Serial.print ("dg/s Z:");
    Serial.print (sensor.getGyroZ(), 1);
    Serial.println ("dg/s");

    Serial.print ("Temperatura:");
    Serial.print (sensor.getTemp(), 1);
    Serial.println ("C");

    Serial.println ();

    nextReading = millis() + 2000;
  }
}
