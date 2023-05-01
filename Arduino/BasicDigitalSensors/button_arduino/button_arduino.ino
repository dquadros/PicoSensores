// Exemplo com Botão

class Button {
  private:
    int pinButton;
    bool pressed;
    int debounce;
    bool last;
    uint32_t lastTime;

  public:

    // Construtor
    Button (int pin, int debounce = 20) {
      pinButton = pin;
      pinMode (pinButton, INPUT_PULLUP);
      pressed = false;
      this->debounce = debounce;
      last = digitalRead(pinButton) == LOW;
      lastTime = millis();
    }

  // Testa se botão está apertado
  bool isPressed() {
    bool val = digitalRead(pinButton) == LOW;
    if (val != last) {
      // reading changed
      last = val;
      lastTime = millis();
    } else if (val != pressed) {
      int dif = millis() - lastTime;
      if (dif > debounce) {
        // update button status
        pressed = val;
      }
    }
    return pressed;
  }
};

// Variáveis Globais
Button button(15);
int counter = 0;

// Iniciações
void setup() {
  Serial.begin(115200);
}

// Laço Principal
void loop() {
  Serial.print ("Botao apertado ");
  Serial.print (counter);
  Serial.println (" vezes");
  while (button.isPressed())
    ;
  while (!button.isPressed())
    ;
  counter++;
}
