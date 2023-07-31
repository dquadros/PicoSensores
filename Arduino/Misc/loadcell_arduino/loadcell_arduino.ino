// Exemplo com Célula de Carga

#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// Acesso ao LCD
LiquidCrystal_PCF8574 lcd(0x27);

// HX711 ADC
const int HX711_DT = 12;
const int HX711_CK = 13;
HX711 sensor;

// Botão
const int BUTTON = 15;

// Iniciação
void setup() {
  // Inicia botão
  pinMode (BUTTON, INPUT_PULLUP);

  // Inicia o display
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();
  lcd.begin(16, 2);
  lcd.setBacklight(255);

  // Calibração inicial
  sensor.begin(HX711_DT, HX711_CK);
  keyPress("Balanca vazia");
  sensor.tare(50);
  keyPress("Coloque 1kg");
  long reading = sensor.read_average(50);
  sensor.set_scale ((reading - sensor.get_offset())/1.00f);
  lcd.clear();
  lcd.print("Balanca Pronta");
}

// Laco Principal
void loop() {
  // lê e mostra o peso
  float weight = sensor.get_units();
  char msg[17];
  dtostrf (weight, 7, 3, msg);
  strcat (msg, "kg");
  lcd.setCursor(0,1);
  lcd.print(msg);
  
  // dá um tempo entre leituras
  delay (500);  
}

// Mostra mensagem e espera a tecla
void keyPress(const char *msg) {
  lcd.clear();
  lcd.print(msg);
  lcd.setCursor(0,1);
  lcd.print("Aperte o botao");
  while (digitalRead(BUTTON) == HIGH) {
    delay(100);
  }
  delay(100); // debounce
  lcd.setCursor(0,1);
  lcd.print("Solte o botao ");
  while (digitalRead(BUTTON) == LOW) {
    delay(100);
  }
  delay(100); // debounce
  lcd.setCursor(0,1);
  lcd.print("Aguarde        ");
}
