// Exemplo com Sensor de Temperatura AHT10
  
#include <Wire.h>  
  
// Endereco I2C do AHT10
#define ADDR      0x38  

// Comandos AHT10
uint8_t cmdInit[] = { 0xE1, 0x08, 0x00 };
uint8_t cmdConv[] = { 0xAC, 0x33, 0x00 };

// Iniciacao
void setup() {  
  Serial.begin (115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();  

  delay(5000);
  
  // Verifica se precisa calibrar
  uint8_t status = getStatus();
  if ((status & 0x08) == 0) {
    Serial.println ("Calibrando");
    Wire.beginTransmission(ADDR);  
    Wire.write(cmdInit, sizeof(cmdInit));  
    Wire.endTransmission();
    delay(10);
  }    
}  
  
// Loop principal: le temperatura e umidade
void loop() {  
  // Dispara conversao
  Wire.beginTransmission(ADDR);  
  Wire.write(cmdConv, sizeof(cmdConv));  
  Wire.endTransmission();

  // Espera conversao
  delay(80);

  // Pega resultado e mostra
  uint16_t r[6];
  float temp, humid;
  Wire.requestFrom(ADDR, 6);
  for (int i = 0; i < 6; i++) {
    r[i] = Wire.read();
  }
  humid = (r[1] << 12) + (r[2] << 4) + (r[3] >> 4);
  humid = (humid / 0x100000) * 100.0;
  temp = ((r[3] & 0x0F) << 16) + (r[4] << 8) + r[5];
  temp = (temp / 0x100000) * 200.0 - 50.0;
  Serial.print("Temperatura: ");
  Serial.print(temp, 1);  
  Serial.print("C Umidade: ");
  Serial.print(humid, 0);  
  Serial.println("\%");  
  delay (2000);
}  
  
// Le status
int8_t getStatus ()  
{  
  Wire.requestFrom(ADDR, 1);  
  return Wire.read();  
} 