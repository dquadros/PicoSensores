// Exemplo com o Sensor de Pressão BMP280/BME280
#include <Arduino.h>
#include <Wire.h>
#include <BMx280I2C.h>

#define I2C_ADDRESS 0x76

BMx280I2C bmx280(I2C_ADDRESS);

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

  // Inicia o sensor
	if (!bmx280.begin()) {
		Serial.println("Nao achou sensor!");
		while (true)
      ;
	}

  // Identifica o sensor
  Serial.println (bmx280.isBME280()? "Sensor BME280" : "Sensor BMP280");

	// coloca o sensor na configuração padrão
	bmx280.resetToDefaults();
	bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
	bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
	if (bmx280.isBME280()) {
		bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
  }
}

// Laco Principal
void loop() {
	delay(2000);
	if (bmx280.measure()) {
    //wait for the measurement to finish
    do {
      delay(10);
    } while (!bmx280.hasValue());

    Serial.print("Temperatura: ");
    Serial.print(bmx280.getTemperature(),2);
    Serial.print("  Pressao: ");
    Serial.print(bmx280.getPressure(),0);
  	if (bmx280.isBME280()) {
		  Serial.print("  Umidade: "); 
		  Serial.print(bmx280.getHumidity());
	  }
    Serial.println();
  }
}
