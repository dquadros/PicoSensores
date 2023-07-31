// Exemplo com o Sensor BMP390
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

// Iniciação
void setup() {
  // Inicia serial
  Serial.begin (115200);
	while (!Serial)
    ;

  // Inicia I2C
  Wire.setSDA(16);
  Wire.setSCL(17);
	Wire.begin();

  // Inicia sensor
	if (!bmp.begin_I2C()) {
		Serial.println("Sensor nao encontrado.");
		while (true)
      ;
	}

  // Configura oversampling e filtro
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

// Laco Principal
void loop() {
	delay(2000);
	if (bmp.performReading()) {
    Serial.print("Temperatura: ");
    Serial.print(bmp.temperature,2);
    Serial.print("C  Pressao: ");
    Serial.print(bmp.pressure,0);
    Serial.print("Pa  Altitude: "); 
    Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println("m");
  }
}
