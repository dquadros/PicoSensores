//Exemplo com Joystick Analogico

// Conexao dos LEDs
#define LED_RIGHT 12
#define LED_UP    13
#define LED_DOWN  14
#define LED_LEFT  15

// Conexcoes do Joystick
#define PIN_HORIZ   A0
#define PIN_VERTIC  A1

//  Ignorar pequenas variacoes do centro
#define ZONA_MORTA 30

// Iniciacao
void setup() {
}

// Laco Principal
void loop() {
  // Le posicao do joystick
  int horiz = analogRead (PIN_HORIZ);
  int vertic = analogRead (PIN_VERTIC);

  // Acende os LEDs conforme a posicao
  if (horiz > (511 + ZONA_MORTA)) {
    analogWrite(LED_RIGHT, 0);
    analogWrite(LED_LEFT, (horiz-512) >> 1);
  } else if (horiz <= (511 - ZONA_MORTA)){
    analogWrite(LED_RIGHT, (511-horiz) >> 1);
    analogWrite(LED_LEFT, 0);
  } else {
    analogWrite(LED_RIGHT, 0);
    analogWrite(LED_LEFT, 0);
  }
  
  if (vertic > (511 + ZONA_MORTA)) {
    analogWrite(LED_DOWN, (vertic-512) >> 1);
    analogWrite(LED_UP, 0);
  } else if (vertic <= (511 - ZONA_MORTA)){
    analogWrite(LED_DOWN, 0);
    analogWrite(LED_UP, (511-vertic) >> 1);
  } else {
    analogWrite(LED_UP, 0);
    analogWrite(LED_DOWN, 0);
  }
}
