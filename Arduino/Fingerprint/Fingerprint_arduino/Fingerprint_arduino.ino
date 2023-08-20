// Exemplo de Uso do Sensor de Digitais

#include <Adafruit_Fingerprint.h>

#define mySerial Serial1
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Conexões do LED RGB
#define LED_R_PIN     13
#define LED_G_PIN     14
#define LED_B_PIN     15

// Iniciação
void setup() {
  // Inicia comunicação com o micro
  while (!Serial)
    ;
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\nExemplo Sensor Digitais\n");

  // Inicia o LED
  pinMode (LED_R_PIN, OUTPUT); digitalWrite(LED_R_PIN, LOW);
  pinMode (LED_G_PIN, OUTPUT); digitalWrite(LED_G_PIN, LOW);
  pinMode (LED_B_PIN, OUTPUT); digitalWrite(LED_B_PIN, LOW);

  // Inicia sensor
  Serial1.setRX(17);
  Serial1.setTX(16);
  finger.begin(57600);

  // Informa a quantidade de digitais que podem ser armazenadas
  Serial.println("Verificando capacidade do sensor");
  finger.getParameters();
  Serial.print("Sensor pode armazenar "); 
  Serial.print(finger.capacity);
  Serial.println(" digitais"); 

  // Limpa digitais armazenadas (se tiver)
  finger.getTemplateCount();
  int count = finger.templateCount;
  if (count > 0) {
    Serial.print("Apagando ");
    Serial.print(count);
    Serial.println(" digitais");
    if (finger.emptyDatabase() == FINGERPRINT_OK) {
      Serial.println ("Sucesso");
    }
  }
}

// Laco Principal
void loop() {
  Serial.println();
  captureFeature(1);
  Serial.println("Procurando...");
  uint8_t ret = finger.fingerSearch();
  if (ret == FINGERPRINT_NOTFOUND) {
    Serial.println("Desconhecida, vamos cadastrar");
    enroll();
  } else if (ret == FINGERPRINT_OK) {
    Serial.print("Digital ");
    Serial.print(finger.fingerID);
    Serial.println(" reconhecida");
  } else {
    Serial.print ("Erro ");
    Serial.println (ret);
  }
}

// Le digital e gera feature
void captureFeature(uint8_t numbuf) {
  while (true) {
    digitalWrite(LED_B_PIN, HIGH);
    Serial.println ("Coloque o dedo no sensor");
    while (finger.getImage() != FINGERPRINT_OK) {
      delay(10);
    }
    digitalWrite(LED_B_PIN, LOW);
    Serial.println("Imagem capturada");

    bool ok = finger.image2Tz(numbuf) == FINGERPRINT_OK;
    Serial.println ("Remova o dedo do sensor");
    delay(2000);
    if (ok) {
      Serial.println ("Feature criada");
      return;
    }
    digitalWrite(LED_R_PIN, HIGH);
    Serial.println ("Imagem ruim, tente de novo");
    delay(1000);
    digitalWrite(LED_R_PIN, LOW);
  }
}

// Cadastra Digital
void enroll() {
  bool first = true;
  while (true) {
    if (!first) {
      captureFeature(1);
    }
    captureFeature(2);
    if (finger.createModel() != FINGERPRINT_OK) {
      digitalWrite(LED_R_PIN, HIGH);
      Serial.println ("Imagens nao combinam, vamos repetir");
      delay(1000);
      digitalWrite(LED_R_PIN, LOW);
      continue;
    }
    finger.getTemplateCount();
    int pos = finger.templateCount;
    int ret = finger.storeModel(pos);
    if (ret == FINGERPRINT_OK) {
      digitalWrite(LED_G_PIN, HIGH);
      Serial.print ("Digital cadastrada na posicao ");
      Serial.println (pos);
      delay(1000);
      digitalWrite(LED_G_PIN, LOW);
      return;
    }
    digitalWrite(LED_R_PIN, HIGH);
    Serial.print ("Erro ");
    Serial.print (ret);
    Serial.println (" ao salvar digital");
    delay(1000);
    digitalWrite(LED_R_PIN, LOW);
  }
}
