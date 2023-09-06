// Exemplo do leitor RFID 125kHz
#include <Servo.h>

// Classe para tratar o Buzzer
class Buzzer {
  private:
    int pinBuzzer;
  public:
    Buzzer(int pin) {
      pinBuzzer = pin;
      pinMode (pinBuzzer, OUTPUT);
      digitalWrite(pinBuzzer, LOW);
    }

    void beep() {
      digitalWrite(pinBuzzer, HIGH);
      delay(300);
      digitalWrite(pinBuzzer, LOW);
    }
};

// Classe para tratar o Botão
class Button {
  private:
    int pinButton;
    bool pressed;
    int debounce;
    bool last;
    uint32_t lastTime;
 public:
    Button(int pin, int debounce = 20) {
      pinButton = pin;
      pinMode (pinButton, INPUT_PULLUP);
      pressed = false;
      this->debounce = debounce;
      last = digitalRead(pinButton) == LOW;
      lastTime = millis();
    }

  // Testa se botão foi apertado e solto
  bool released() {
    bool val = digitalRead(pinButton) == LOW;
    if (val != last) {
      // leitura mudou
      last = val;
      lastTime = millis();
    } else if (val != pressed) {
      int dif = millis() - lastTime;
      if (dif > debounce) {
        // atualiza estado do botao
        pressed = val;
        return !pressed;
      }
    }
    return false;
  }
};

// Define qual UART usar (Serial1 ou Serial2)
#define SERIAL_RFID Serial1

// Classe para ler tags
class RFID {
  private:
    byte last[9];
    uint32_t last_read;
    byte bufRx[14];
    int pos;

    inline byte decodHex(byte c) {
      if ((c >= '0') && (c <= '9')) {
        return c - '0';
      }
      if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
      }
      return 0;
    }

  public:
    RFID(int pinRx, int pinTx) {
      SERIAL_RFID.setRX(pinRx);
      SERIAL_RFID.setTX(pinTx);
      SERIAL_RFID.begin(9600, SERIAL_8N1);
      pos = 0;
      last[0] = 0;
      last_read = 0;
    }

    bool read(char *tag) {
      if (SERIAL_RFID.available() == 0) {
        uint32_t ellapsed = millis() - last_read;
        if ((last[0] != 0) && (ellapsed > 1000)) {
          // Muito tempo sem ler, esquece o tag anterior
          last[0] = 0;
        }
        return false;
      }
      int c = SERIAL_RFID.read();
      if (c < 0) {
        return false;
      }
      if ((pos == 0) && (c != 0x02)) {
        return false;
      }
      bufRx[pos++] = (byte) c;
      if (pos == 14) {
        pos = 0;
        if (c == 0x03) {
          last_read = millis();
          byte crc = 0;
          byte x;
          for (int i = 1; i < 13; i = i+2) {
            x = (decodHex(bufRx[i]) << 4) + decodHex(bufRx[i+1]);
            crc ^= x;
          }
          if (crc == 0) {
            if (memcmp (bufRx+3, last, 8) != 0) {
              memcpy(last, bufRx+3, 8);
              last[8] = 0;
              strcpy(tag, (const char *)last);
              return true;
            }
          }
        }
      }
      return false;
    }

};

// Declara e cria objetos
Buzzer buzzer(2);
Button button(16);
Servo servo;
RFID *rfid;

#define MAX_TAGS 10
char autorized[MAX_TAGS][9];
int nAuthorized = 0;
bool cadastro = false;
uint32_t fechar = 0;

// Iniciação
void setup() {
  while (!Serial) {
    delay(100);
  }
  Serial.println("Exemplo RFID 125kHz\n");
  rfid = new RFID(13, 12);
  servo.attach(17);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  buzzer.beep();
  servo.write(0);
  digitalWrite(LED_BUILTIN, LOW);
}

// Laço principal
void loop() {
  char tag[9];

  if ((fechar != 0) && (millis() > fechar)) {
    servo.write(0);
    fechar = 0;
  }
  if (button.released()) {
    cadastro = true;
  }
  if (rfid->read(tag)) {
    if (cadastro) {
      if (isAuthorized(tag)) {
        Serial.print ("Tag ");
        Serial.print (tag);
        Serial.println (" ja estava cadastrada");
      } else if (nAuthorized == MAX_TAGS) {
        Serial.println ("Lista de tags autorizadas esta cheia");
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
        strcpy (autorized[nAuthorized++], tag);
        Serial.print ("Tag ");
        Serial.print (tag);
        Serial.println (" cadastrada");
        delay(300);
        digitalWrite(LED_BUILTIN, LOW);
      }
    cadastro = false;
    }
    else if (isAuthorized(tag)) {
        Serial.print ("Tag ");
        Serial.print (tag);
        Serial.println (" autorizada");
        servo.write(180); // "abre a porta"
        fechar = millis()+3000;
    } else {
        Serial.print ("Tag ");
        Serial.print (tag);
        Serial.println (" NAO autorizada");
        buzzer.beep();
    }
  }
}

// Verifica se tag está na lista
bool isAuthorized(char * tag) {
  for (int i = 0; i < nAuthorized; i++) {
    if (strcmp(autorized[i], tag) == 0) {
      return true;
    }
  }
  return false;
}
