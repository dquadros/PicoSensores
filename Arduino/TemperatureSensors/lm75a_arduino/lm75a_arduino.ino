// Exemplo com Sensor de Temperatura LM75A
  
#include <Wire.h>  
  
// Endereco I2C do LM75A 
#define ADDR      0x48  
  
// Registradores do LM75A
#define REG_TEMP  0  
#define REG_CONF  1  
#define REG_THYST 2  
#define REG_TOS   3  
  
// Iniciacao
void setup() {  
  Serial.begin (115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();  
  
  // Configura o sensor e define limites para a saida OS  
  WriteReg8 (REG_CONF, 0);
  WriteReg16 (REG_TOS, EncodeTemp(22.5));  
  WriteReg16 (REG_THYST, EncodeTemp(20.0));  
}  
  
// Laco principal: le a temperatura
void loop() {  
  delay (500);
  Serial.print("Temperature: ");
  Serial.print(DecodeTemp(ReadReg16(REG_TEMP)));  
  Serial.println("C");
}  
  
// Codifica temperatura para o sensor
int16_t EncodeTemp (float temp)  
{
  if (temp >= 0) {
    return ((int16_t) (temp / 0.5)) << 7;
  }  else {
    return (512 + (int16_t) (temp / 0.5)) << 7;
  }
}
  
// Decodifica temperatura informada pelo sensor
float DecodeTemp (int16_t val)  
{  
  val = val / 32;
  if (val >= 1024) {
    return ((float) (val-2048)) * 0.125;
  } else {
    return ((float) val) * 0.125;
  }
}  
  
//Escreve valor de 8 bits em um registrador
void WriteReg8 (byte reg, int8_t val)  
{  
  Wire.beginTransmission(ADDR);  
  Wire.write(reg);  
  Wire.write(val);  
  Wire.endTransmission();  
}  
  
//Escreve valor de 16 bits em um registrador
void WriteReg16 (byte reg, int16_t val)  
{  
  Wire.beginTransmission(ADDR);  
  Wire.write(reg);  
  Wire.write((val >> 8) & 0xFF);  
  Wire.write(val & 0xFF);  
  Wire.endTransmission();  
}  
  
// Le valor de 16 bits em um registrador
int16_t ReadReg16 (byte reg)  
{  
  uint16_t val;  
    
  // Seleciona registrador
  Wire.beginTransmission(ADDR);  
  Wire.write(reg);  
  Wire.endTransmission();  
  
  // Le o valor
  Wire.requestFrom(ADDR, 2);  
  val = Wire.read() << 8;  
  val |= Wire.read();  
  return (int16_t) val;  
} 