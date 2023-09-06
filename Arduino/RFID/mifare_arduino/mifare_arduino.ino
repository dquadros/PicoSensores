// Exemplo de Bilhetagem com Leitor RFID MIFARE MFRC522

#include <SPI.h>
#include <MFRC522.h>
 
// Acesso ao módulo Rfid
const byte RST_PIN = 20;  // pino de reset
const byte SS_PIN = 17;   // pino de seleção
MFRC522 mfrc522(SS_PIN, RST_PIN);
 
// Chaves para acesso ao cartão
typedef enum { FACTORY, OUR_APP, OTHER } KEY_TYPE;
MFRC522::MIFARE_Key defaultKey;
MFRC522::MIFARE_Key appKeyA;
MFRC522::MIFARE_Key appKeyB;
KEY_TYPE cardKey;  // chave do cartão em uso
 
// Seleciona o Setor e Bloco que será usado
const byte SECTOR = 2;
const byte VALUE_BLK = 8;
const byte CFG_BLK = 11;
 
// Valores de carga e consumo
const int32_t INITIAL_VALUE = 30;
const int32_t CHARGE_VALUE = 7;
 
// Iniciação
void setup() {
  while (!Serial) {
    delay(100);
  }

  // Inicia a porta serial
  Serial.begin (115200);
 
  // Inicia o acesso ao módulo
  SPI.begin();
  mfrc522.PCD_Init();
 
  // Inicia as chaves
  for (byte i = 0; i < 6; i++) {
    defaultKey.keyByte[i] = 0xFF;
  }
  appKeyA.keyByte[0] = 'S';  appKeyA.keyByte[1] = 'e';  appKeyA.keyByte[2] = 'c';
  appKeyA.keyByte[3] = 'r';  appKeyA.keyByte[4] = 'e';  appKeyA.keyByte[5] = 't';
  appKeyB.keyByte[0] = '1';  appKeyB.keyByte[1] = '2';  appKeyB.keyByte[2] = '3';
  appKeyB.keyByte[3] = '4';  appKeyB.keyByte[4] = '5';  appKeyB.keyByte[5] = '6';
}
 
// Laço principal
void loop() {
  // Solicita uma opção
  Serial.print ("(I)nicia, (C)onsome ou (F)abrica?");
  int c;
  while (Serial.read() != -1) {
    delay(10);
  }
  do {
    c = Serial.read();
    c = toupper(c);
  } while (strchr("ICF", c) == NULL);
  char cmd[2] = "";
  cmd[0] = (char) c;
  Serial.println (cmd);
 
  // Pede para aproximar um cartão
  if (askForCard()) {
    // Trata a opção
    if (c == 'I') {
      initCard();
    } else if (c == 'C') {
      useCard();
    } else if (c == 'F') {
      resetCard();
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
  Serial.println();
}
 
// Inicia o cartão para uso
void initCard() {
  MFRC522::StatusCode status;
   
  // Se autentica com a chave B
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, CFG_BLK, 
      (cardKey == FACTORY)? &defaultKey : &appKeyB, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro na autenticacao!");
    return;
  }
 
  // Grava o setor de valor com o formato correto
  if (!setupValue(VALUE_BLK)) {
    return;
  }
 
  // Reconfigura o setor para usar nossas chaves e as condições de acesso desejadas
  // 1o bloco 000 configuração padrão, pode acessar com chaves A ou B
  // 2o bloco 110 valor, leitura/decremento com chave A ou B, escrita/incremento só com B
  // 3o bloco 000 configuração padrão, pode acessar com chaves A ou B
  // 4o bloco 011 escrita só com B, chave A só lê bits de acesso
  byte cfgBuffer[16];
  memset (cfgBuffer, 0, 16);
  memcpy (cfgBuffer, &appKeyA, 6);
  memcpy (cfgBuffer+10, &appKeyB, 6);
  mfrc522.MIFARE_SetAccessBits(&cfgBuffer[6], 0, 6, 0, 3);
  status = mfrc522.MIFARE_Write(CFG_BLK, cfgBuffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro ao configurar!");
  }

  Serial.println ("Cartao iniciado");
  cardKey = OUR_APP;

  // Se autentica com a chavenova  B
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, CFG_BLK, 
      &appKeyB, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro na autenticacao!");
    return;
  }
 
  status = mfrc522.MIFARE_Increment(VALUE_BLK, INITIAL_VALUE);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro ao carregar o valor inicial!");
    return;
  }
  status = mfrc522.MIFARE_Transfer(VALUE_BLK);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro na salvar novo saldo!");
    return;
  }
 
  // Mostra o saldo inicial
  showBalance();
}
 
// Consome CHARGE_VALUE "créditos"
void useCard() {
  MFRC522::StatusCode status;
 
  if (cardKey != OUR_APP) {
    Serial.println ("Cartao nao iniciado!");
    return;
  }
   
  // Se autentica com a chave A
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, CFG_BLK, 
      &appKeyA, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro na autenticacao!");
    return;
  }
 
  // Vê se tem saldo suficente
  int32_t value;
  status = mfrc522.MIFARE_GetValue(VALUE_BLK, &value);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro na leitura!");
    return;
  }
  if (value < CHARGE_VALUE) {
    Serial.println ("Saldo insuficiente!");
    return;
  }
 
  // Atualiza o saldo
  status = mfrc522.MIFARE_Decrement(VALUE_BLK, CHARGE_VALUE);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro ao debitar!");
    return;
  }
  status = mfrc522.MIFARE_Transfer(VALUE_BLK);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro na salvar novo saldo!");
    return;
  }
 
  // Mostra o novo saldo
  showBalance();
}
 
// Informa o saldo
void showBalance() {
  MFRC522::StatusCode status;
 
  if (cardKey != OUR_APP) {
    Serial.println ("Cartao nao iniciado!");
    return;
  }
   
  // Se autentica com a chave A
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, CFG_BLK, 
      &appKeyA, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro na autenticacao!");
    return;
  }
 
  // Le o Valor
  int32_t value;
  status = mfrc522.MIFARE_GetValue(VALUE_BLK, &value);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro na leitura!");
    return;
  }
  Serial.print ("Saldo: ");
  Serial.println (value);
}
 
// Recoloca o cartão na condição de fábrica
void resetCard() {
  MFRC522::StatusCode status;
 
  // Se autentica com a cardKey B
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, CFG_BLK, 
      (cardKey == FACTORY)? &defaultKey : &appKeyB, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro na autenticacao!");
    return;
  }
   
  // Reconfigura o setor para a configuração padrão
  byte cfgBuffer[16];
  memset (cfgBuffer, 0, 16);
  memcpy (cfgBuffer, &defaultKey, 6);
  memcpy (cfgBuffer+10, &defaultKey, 6);
  mfrc522.MIFARE_SetAccessBits(&cfgBuffer[6], 0, 0, 0, 1);
  status = mfrc522.MIFARE_Write(CFG_BLK, cfgBuffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Erro ao configurar!");
  }
  Serial.println ("Cartao de volta a condicao de fabrica");
}
 
// Aguarda apresentar um cartão ou digitar algo
bool askForCard() {
  while (Serial.read() != -1)
    ;
  Serial.println ("Aproxime um cartao (digite qualquer tecla para desistir)");
  while (true) {
    if (Serial.read() != -1) {
      return false;
    }
    if (!mfrc522.PICC_IsNewCardPresent())
      continue;
    if (mfrc522.PICC_ReadCardSerial())
      break;
  }

  Serial.print ("ID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
   
  Serial.print(F("Tipo: "));
  MFRC522::PICC_Type tipo = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(tipo));
 
  // Verifica se tipo aceito
  if ( (tipo == MFRC522::PICC_TYPE_MIFARE_MINI) ||
       (tipo == MFRC522::PICC_TYPE_MIFARE_1K) ||
       (tipo == MFRC522::PICC_TYPE_MIFARE_4K) ) {
    cardKey = checkKey();
    return cardKey != OTHER;
  }
  Serial.println ("Cartao incompativel!");
  return false;
}
 
// Verifica qual a chave usada no nosso setor
KEY_TYPE checkKey() {
  MFRC522::StatusCode status;
 
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, CFG_BLK, 
      &defaultKey, &(mfrc522.uid));
  if (status == MFRC522::STATUS_OK) {
    Serial.println ("Usa chave da fábrica");
    return FACTORY;
  }
 
  // Reinicia a comunicação, para poder testar outra chave
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  mfrc522.PICC_IsNewCardPresent();
  mfrc522.PICC_ReadCardSerial();
   
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, CFG_BLK, 
      &appKeyA, &(mfrc522.uid));
  if (status == MFRC522::STATUS_OK) {
    Serial.println ("Usa a nossa chave");
    return OUR_APP;
  }
   
  Serial.println ("Chave desconhecida!");
  return OTHER;
}
 
// Inicia um bloco no formato apropriado para valor
bool setupValue(byte block) {
  MFRC522::StatusCode status;
  byte valueBlock[] = {
      0, 0, 0, 0,
      255, 255, 255, 255,
      0, 0, 0, 0,
      block, (byte) ~block, block, (byte) ~block };
  status = mfrc522.MIFARE_Write(block, valueBlock, 16);
  if (status != MFRC522::STATUS_OK) {
      Serial.print("Erro ao gravar bloco!");
      return false;
  }
  return true;
}
