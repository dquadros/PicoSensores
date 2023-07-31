// Exemplo para QMC5883L & HMC5883L

#include <DFRobot_QMC5883.h>

//DFRobot_QMC5883 compass(&Wire, QMC5883_ADDRESS);
DFRobot_QMC5883 compass(&Wire, HMC5883L_ADDRESS);


// Iniciação
void setup() {
  while(!Serial) {
    delay(100);
  }
  Serial.begin(115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  while (!compass.begin())
  {
    Serial.println("Nao achei o sensor, confira as ligacoes!");
    delay(2000);
  }
  Serial.println(compass.isHMC() ? "HMC5883L" : "QMC5883L");
}

// Laco principal
void loop() {
  // Lê o dado do sensor e obtem referência ao vetor interno
  sVector_t mag = compass.readRaw();
  // Completa o vetor com a direção
  compass.getHeadingDegrees();
  Serial.print("Direcao: ");
  Serial.println(mag.HeadingDegress,1);
  delay(2000);
}
