// Exemplo com Teclado

// Pinos do Teclado
#define NROWS     4
#define NCOLUMNS  3
#define ROW_1     0
#define ROW_2     1
#define ROW_3     2
#define ROW_4     3
#define COL_1     4
#define COL_2     5
#define COL_3     6

// Debounce
#define DEBOUNCE 5

// Variáveis de controle do teclado
int rows[] = { ROW_1, ROW_2, ROW_3, ROW_4 };
int columns[] = { COL_1, COL_2, COL_3 };
int curRow;
bool previous [NROWS][NCOLUMNS];
bool validated [NROWS][NCOLUMNS];
int count [NROWS][NCOLUMNS];

// Código das teclas
char codes[NROWS][NCOLUMNS] = { 
 { '1', '2', '3' },
 { '4', '5', '6' },
 { '7', '8', '9' },
 { '*', '0', '#' }
};

// Fila do teclado
#define MAX_KEYS 16
char keys[MAX_KEYS+1];
int inkey, outkey;

// Verifica teclas apertadas na linha atual
void checkRow() {
  // coloca a linha atual em nível BAIXO
  int pin = rows[curRow];
  pinMode (pin, OUTPUT);
  digitalWrite(pin, LOW);

  // Lê as colunas e verifica mudanças
  for (int col = 0; col < NCOLUMNS; col++) {
    bool read = digitalRead (columns[col]) == LOW;
    if (read == previous[curRow][col]) {
      if (count[curRow][col] != 0) {
        if (--count[curRow][col] == 0) {
          // reading validated
          if (read != validated[curRow][col]) {
            validated[curRow][col] = read;
            if (read) {
              // keypress detected
              int aux = inkey+1;
              if (aux > MAX_KEYS) {
                aux = 0;
              }
              if (aux != outkey) {
                keys[inkey] = codes[curRow][col];
                inkey = aux;
              }
            }
          }
        }
      }
    } else {
      // reinicia a validação
      previous[curRow][col] = read;
      count[curRow][col] = DEBOUNCE;
    }
  }
  // reconfigura a linha como entrada
  pinMode (pin, INPUT);
  // passa para a próxima linha
  curRow = (curRow+1) % NROWS;
}

// Lê uma tecla da fila, retorna -1 se fila vazia
int getKey(void) {
    int key = -1;
    if (inkey != outkey) {
        key = keys[outkey];
        outkey = (outkey == MAX_KEYS) ? 0 : (outkey + 1);
    }
    return key;
}

// Iniciações
void setup() {
  Serial.begin(115200);
  Serial.println("Exemplo de Teclado");

  // inicia controles do teclado
  curRow = 0;
  for (int i = 0; i < NROWS; i++) {
    for (int j = 0; j < NCOLUMNS; j++) {
      previous[i][j] = validated[i][j] = false;
      count[i][j] = DEBOUNCE;
    }
  }

  // inicia fila de teclas
  inkey = outkey = 0;

  // inicia pinos do teclado
  for (int i = 0; i < NROWS; i++) {
    pinMode(rows[i], INPUT);
  }
  for (int i = 0; i < NCOLUMNS; i++) {
    pinMode(columns[i], INPUT_PULLUP);
  }
}

// Laço Principal
void loop() {
  checkRow();
  int key = getKey();
  if (key == -1) {
    delay(10);
  } else {
    Serial.print ("Apertou ");
    Serial.println ((char) key);
    Serial.flush ();
  }
}
