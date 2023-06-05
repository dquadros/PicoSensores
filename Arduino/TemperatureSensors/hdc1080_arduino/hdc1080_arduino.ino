// Exemplo com Sensor de Temperatura HDC1080
  
#include <Wire.h>  
  
// Endereco I2C do HDC1080
#define ADDR      0x40  
  
// Registradores do HDC1080
#define REG_TEMP  0  
#define REG_HUM   1  
#define REG_CONF  2  
#define REG_MANID 0xFE
#define REG_DEVID 0xFF
  
// Iniciacao
void setup() {  
  Serial.begin (115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();  

  delay(5000);
  
  // Verifica IDs de Fabricante e Dispositivo
  uint16_t manID = ReadReg16(REG_MANID);
  uint16_t devID = ReadReg16(REG_DEVID);
  Serial.print ("Fabricante: ");
  Serial.print (manID, HEX);
  Serial.print (" Dispositivo: ");
  Serial.println (devID, HEX);
}  
  
// Loop principal: le temperatura
void loop() {  
  // Dispara conversao
  Wire.beginTransmission(ADDR);  
  Wire.write(REG_TEMP);  
  Wire.endTransmission();

  // Espera conversao
  delay(20);

  // Pega resultado e mostra
  uint16_t r;
  float temp, humid;
  Wire.requestFrom(ADDR, 4);
  r = Wire.read() << 8;
  r |= Wire.read();  
  temp = r*165.0/65536.0 - 40.0;
  r = Wire.read() << 8;
  r |= Wire.read();  
  humid = r*100.0/65536.0;
  Serial.print("Temperatura: ");
  Serial.print(temp, 1);  
  Serial.print("C Umidade: ");
  Serial.print(humid, 0);  
  Serial.println("\%");  
  delay (2000);
}  
  
// Le registrador de 16 bits
int16_t ReadReg16 (byte reg)  
{  
  uint16_t val;  
    
  // Seleciona register
  Wire.beginTransmission(ADDR);  
  Wire.write(reg);  
  Wire.endTransmission();  
  
  // Le valor
  Wire.requestFrom(ADDR, 2);  
  val = Wire.read() << 8;  
  val |= Wire.read();  
  return (int16_t) val;  
} 