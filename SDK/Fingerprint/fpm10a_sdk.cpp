/**
 * @file fpm10a_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Driver Simples para Sensor de Digitais FPM10A
 * @version 1.0
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "fpm10a_sdk.h"

// Construtor
FPM10A::FPM10A(uart_inst_t *serial) {
  uart = serial;
}

// Inicia comunicação
void FPM10A::begin() {
  uart_init (uart, 9600*6);
  uart_set_format (uart, 8, 1, UART_PARITY_NONE);
  sleep_ms (500);
  response = RESP_OK;
}

// Informa ultima resposta
uint8_t FPM10A::lastResponse() {
  return response;
}

// Le a configuração
bool FPM10A::leSysParam (SYSPARAM *param) {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_READSYSPARAM;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  if (getResponse() && (resp.response == RESP_OK)) {
    memcpy (param, &resp.d.sysparam, sizeof(SYSPARAM));
    return true;
  } else {
    return false;
  }
}

// Determina quantas digitais tem armazenadas no sensor
// Retorna -1 se algo der errado
int FPM10A::count() {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_TEMPLATECOUNT;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  if (getResponse() && (resp.response == RESP_OK)) {
    return (resp.d.templcount[0] << 8)+ resp.d.templcount[1];
  } else {
    return -1;
  }
}

// Captura a imagem de uma digital
bool FPM10A::capture () {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_GENIMG;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

/**
 * Converte a imagem atual em um feature template e 
 * coloca em CharBuffer 'numBuf' (1 ou 2)
 */
bool FPM10A::createFeature (uint8_t numBuf) {
  CMDIMG2TZ cmd;
  cmd.cmd = CMD_IMG2TZ;
  cmd.bufno = numBuf;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

// Cria um modelo (template) a partir das duas features em CharBuffer
bool FPM10A::createModel () {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_REGMODEL;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

/**
 * Salva o modelo que está em CharBuffer 'numBuf' (1 ou 2) na posição
 * 'pos' (0 a 149) da memória do módulo
 */
bool FPM10A::store (uint8_t numBuf, int pos) {
  CMDSTORE cmd;
  cmd.cmd = CMD_STORE;
  cmd.bufno = numBuf;
  cmd.pos[0] = pos >> 8;
  cmd.pos[1] = pos & 0xFF;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

/**
 * Carrega em CharBuffer 'numBuf' (1 ou 2) o modelo que está na posição
 * 'pos' (0 a 149) da memória do módulo
 */
bool FPM10A::load (uint8_t numBuf, int pos) {
  CMDLOAD cmd;
  cmd.cmd = CMD_LOAD;
  cmd.bufno = numBuf;
  cmd.pos[0] = pos >> 8;
  cmd.pos[1] = pos & 0xFF;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

// Compara as digitais em CharBuffer 1 e 2
 bool FPM10A::match () {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_MATCH;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

/**
 * Procura na biblioteca a digital em CharBuffer 'numBuf' (1 ou 2)
 * Se achar, retorna a pos (0 a 149)
 * Se não achar (ou der erro) retorna -1
 */
int FPM10A::search (uint8_t numBuf) {
  CMDSEARCH cmd;
  int ndigitals = count();
  cmd.cmd = CMD_SEARCH;
  cmd.bufno = numBuf;
  cmd.start[0] = 0;
  cmd.start[1] = 0;
  cmd.qty[0] = ndigitals >> 8;
  cmd.qty[1] = ndigitals & 0xFF;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  if (getResponse() && (resp.response == RESP_OK)) {
    return (resp.d.search.pos[0] << 8) + resp.d.search.pos[1];
  } else {
    return -1;
  }
}

// Apaga todas as digitais armazenadas
bool FPM10A::clear () {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_EMPTY;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

// Envia um comando para o sensor
void FPM10A::sendCmd (uint8_t *cmd, int size) {
  // monta o cabeçalho e envia
  header.startHi = START >> 8;
  header.startLo = START & 0xFF;
  header.addr3 = 0xFF;  // endereço default
  header.addr2 = 0xFF;
  header.addr1 = 0xFF;
  header.addr0 = 0xFF;
  header.type = 0x01; // comando
  header.lenHi = (size+2) >> 8;
  header.lenLo = (size+2) & 0xFF;
  uart_write_blocking (uart, (const uint8_t *) &header, sizeof(header));

  // calcula o checksum
  uint16_t chksum = header.type + header.lenHi + header.lenLo;
  for (int i = 0; i < size; i++) {
    chksum += cmd[i];
  }

  // envia o comando
  uart_write_blocking (uart, (const uint8_t *) cmd, size);
  
  // envia o checksum
  uart_putc_raw (uart, chksum >> 8);
  uart_putc_raw (uart, chksum & 0xFF);
}

// Recebe uma resposta do sensor
bool FPM10A::getResponse() {
  enum { START1, START2, ADDR, TAG, LEN1, LEN2, DATA, CHECK1, CHECK2 } state = START1;
  uint16_t checksum;
  uint8_t *p = (uint8_t *) &resp;
  int size;
  int i;
  bool ok = false;
  while (true) {
    int c = uart_getc(uart);
    //printf (" %02X", c);
    *p++ = c;
    switch (state) {
      case START1:
        if (c == (START >> 8)) {
          state = START2;
        } else {
          p = (uint8_t *) &resp; // ignora o uint8_t lido
        }
        break;
      case START2:
        if (c == (START & 0xFF)) {
          state = ADDR;
          i = 0;
        } else {
          state = START1;
          p = (uint8_t *) &resp; // ignora os uint8_ts lidos
        }
        break;
      case ADDR:
        if (++i == 4) {
          state = TAG;
        }
        break;
      case TAG:
        checksum = c;
        state = LEN1;
        break;
      case LEN1:
        checksum += c;
        size = c << 8;
        state = LEN2;
        break;
      case LEN2:
        checksum += c;
        size += c - 2;  // desconta o checksum
        state = DATA;
        i = 0;
        break;
      case DATA:
        checksum += c;
        if (++i == size) {
          state = CHECK1;
        }
        break;
      case CHECK1:
        ok = c == (checksum >> 8);
        state = CHECK2;
        break;
      case CHECK2:
        ok = ok && (c == (checksum & 0xFF));
        if (ok) {
          response = resp.response;
        }
        //printf("\n");
        return ok;
    }
  }
}
