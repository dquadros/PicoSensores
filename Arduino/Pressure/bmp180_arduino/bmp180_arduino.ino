// Exemplo com o Sensor de Pressão BMP085/BMP180
// Adaptado do Exemplo da Adafruit

#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

// Iniciação
void setup() {
  Serial.begin (115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  if (!bmp.begin()) {
    Serial.println("Nao achou sensor BMP085/BMP180!");
    while (true) {
    }
  }
}

// Laço Principal
void loop() {
  Serial.print("Temperatura: ");
  Serial.print(bmp.readTemperature());
  Serial.print(" C");
  
  Serial.print("  Pressao: ");
  Serial.print(bmp.readPressure());
  Serial.print(" Pa");
  
  // Calcula a altitude usando a pressao padrao
  // de 1013.25 millibar = 101325 Pascal
  Serial.print("  Altitude: ");
  Serial.print(bmp.readAltitude());
  Serial.println(" m");

  delay(2000);
}
