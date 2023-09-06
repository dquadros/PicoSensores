# Exemplo RFID com PN532

import board
import busio
import re
from adafruit_pn532.i2c import PN532_I2C
from adafruit_pn532.adafruit_pn532 import MIFARE_CMD_AUTH_A

# Inicia PN532
i2c = busio.I2C(sda=board.GP16,scl=board.GP17)
pn532 = PN532_I2C(i2c, debug=False)
ic, ver, rev, support = pn532.firmware_version
print('Found PN532 with firmware version: {0}.{1}'.format(ver, rev))
pn532.SAM_configuration()

# Marca e chave
mark = bytes([ 0x00, 0x11, 0x22, 0x33, 0x44, 0x66, 0x66, 0x77,
         0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF])
cardKey = bytes([ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ])


# Aguarda cartao
def waitForCard():
    print ('Aproxime o cartao')
    while True:
        uid = pn532.read_passive_target(timeout=0.5)
        if uid is None:
            continue
        print("ID do cartão: {}".format(
            hex(int.from_bytes(uid,"little")).upper()))
        return uid

# Grava no cartao
def writeToCard(param):
    uid = waitForCard()
    block = int(param[1])*4
    msg = bytes((param[2]+16*' '),'utf-8')[0:16]
    if pn532.mifare_classic_authenticate_block(uid, block, MIFARE_CMD_AUTH_A, cardKey) and \
       pn532.mifare_classic_write_block(block, mark) and \
       pn532.mifare_classic_write_block(block+1, msg):
           print ('Gravado.')
    else:
        print ('Erro gravando no cartao!')

# Le do cartao
def readFromCard(param):
    uid = waitForCard()
    block = int(param[1])*4
    if pn532.mifare_classic_authenticate_block(uid, block, MIFARE_CMD_AUTH_A, cardKey):
        data = pn532.mifare_classic_read_block(block)
        if data == mark:
            msg = pn532.mifare_classic_read_block(block+1)
            if msg != None:
                print ('Msg: '+msg.decode('utf-8'))
                return
        elif data != None:
            print ('Setor nao gravado!')
            return
    print ('Erro na leitura!')

# Laco Principal
cmdRead = "^([lL]) ([1-9])$"
cmdWrite = "^([gG]) ([1-9]) (.+)$"
while True:
    # Lê comando
    print()
    cmd = input("Comando? ")
    m = re.search(cmdRead, cmd)
    if m != None:
        readFromCard(m.groups())
    else:
        m = re.search(cmdWrite, cmd)
        if m != None:
            writeToCard(m.groups())
        else:
            print ('Comando desconhecido')
