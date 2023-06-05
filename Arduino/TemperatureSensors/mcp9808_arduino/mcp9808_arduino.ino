// Exemplo com Sensor MCP9808
  
#include <Wire.h>  
  
// Endereco I2C do  MCP9808 
#define ADDR      0x18  
  
// Registradores do MCP9808
#define REG_CONFIG  1
#define REG_UPPER   2  
#define REG_LOWER   3
#define REG_CRIT    4
#define REG_TA      5
#define REG_MANID   6
#define REG_DEVID   7
#define REG_RESOL   8
  
// Iniciacao
void setup() {  
  Serial.begin (115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();  
  
  // Verifica IDs de fabricante e dispositivo
  uint16_t manID = ReadReg16(REG_MANID);
  uint16_t devID = ReadReg16(REG_DEVID);
  Serial.print ("Fabricante: ");
  Serial.println (manID, HEX);
  Serial.print ("Dispositivo: ");
  Serial.print (devID >> 8, HEX);
  Serial.print (" rev ");
  Serial.println (devID & 0xFF);

  // Define limites para saida Alert
  WriteReg16(REG_CRIT,EncodeTemp(30.0));
  WriteReg16(REG_UPPER,EncodeTemp(23.0));
  WriteReg16(REG_LOWER,EncodeTemp(20.0));
  WriteReg16(REG_CONFIG,0x0008);
}  
  
// Laco principal: le temperatura
void loop() {  
  delay (500);
  Serial.print("Temperatura: ");
  Serial.print(DecodeTemp(ReadReg16(REG_TA)));  
  Serial.println("C");
}  
  
// Codifica temperatura para o sensor
int16_t EncodeTemp (float temp)  
{
  if (temp >= 0) {
    return ((uint16_t) (temp/0.25)) << 2;
  } else {
    return ((uint16_t) (2048 + temp/0.25)) << 2;
  }  
}  
  
//Decodifica temperatura lida do sensor
float DecodeTemp (int16_t val)  
{  
  bool sign = (val & 0x1000) != 0;
  val = val & 0x1FFF;
  if (sign) {
    val = val - 0x2000;
  }
  return (float)val/16.0;
}  
  
// Escreve valor de 16 bits em um registrador
void WriteReg16 (byte reg, int16_t val)  
{  
  Wire.beginTransmission(ADDR);  
  Wire.write(reg);  
  Wire.write((val >> 8) & 0xFF);  
  Wire.write(val & 0xFF);  
  Wire.endTransmission();  
}  
  
// Le registrador de 16 bits
int16_t ReadReg16 (byte reg)  
{  
  uint16_t val;  
    
  // Seleciona registrador
  Wire.beginTransmission(ADDR);  
  Wire.write(reg);  
  Wire.endTransmission();  
  
  // Le valor
  Wire.requestFrom(ADDR, 2);  
  val = Wire.read() << 8;  
  val |= Wire.read();  
  return (int16_t) val;  
} 