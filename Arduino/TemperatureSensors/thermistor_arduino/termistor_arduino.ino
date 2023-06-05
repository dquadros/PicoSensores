// Exemplo com Termistor

#include <math.h>

// Conexão do termistor
const int pinTermistor = A0;  

// Parâmetros do Circuito
const float Vcc = 3.3;  
const float R = 2200.0;  

// Parâmetros do Termistor
float beta;  
float rx;  
float rt1, t1, rt2, t2;
  
// Retorna a resistência do termistor
// usa a média de 20 leituras do ADC
float getResistance() {
  uint32_t sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += analogRead(pinTermistor);
  }
  float v = (Vcc*sum)/(20*1024.0);
  return R*(Vcc-v)/v;
}

// Obtem uma temperatura de referência
void getReference(float &t, float &r) {
  Serial.print("Temperatura: ");
  // Espera a resposta
  while (Serial.available() == 0) {
    delay(100);
  }
  t = Serial.parseFloat();
  Serial.print(t, 1);
  t += 273.0;
  r = getResistance();
  Serial.print(" Resistencia: ");
  Serial.println(r, 0);
}

// Iniciação
void setup() {  
  Serial.begin(115200);
  Serial.setTimeout(10000);
  // Espera apertar Enter
  while (Serial.read() == -1) {
    delay(100);
  }

  // Obtem duas referências
  Serial.println("Referencia 1");
  getReference (t1, rt1);
  Serial.println("Referencia 2");
  getReference (t2, rt2);

  // Calcula beta e rx
  beta = log(rt1/rt2)/((1/t1)-(1/t2));
  Serial.print("Beta = ");
  Serial.println(beta, 2);
  rx = rt1 * exp(-beta/t1);
}  
  
// Laço principal
void loop() {  
  float rt = getResistance();
  float t = beta / log(rt/rx);
  Serial.print("Temperatura: ");
  Serial.println(t-273.0, 1);
  delay (1000);  
} 