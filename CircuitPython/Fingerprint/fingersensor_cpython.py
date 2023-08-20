# Driver Simplificado para o Sensor de Digitais FPM10A
from busio import UART
from time import time,sleep

class FingerSensor(object):
    
    # Codigos de Resposta
    RESP_OK = 0
    RESP_ERRCOM = 1
    RESP_NOFINGER = 2
    RESP_IMGFAIL = 3
    RESP_BADIMG = 6
    RESP_FEWFP = 7
    RESP_TOODIF = 10
    RESP_NOIMG = 21
    
    # Comandos
    CMD_GENIMG = 0x01
    CMD_IMG2TZ = 0x02
    CMD_SEARCH = 0x04
    CMD_REGMODEL = 0x05
    CMD_STORE = 0x06
    CMD_EMPTY = 0x0D
    CMD_TEMPLATECOUNT = 0x1D
    
    # Construtor
    def __init__(self, uart):
        self.uart = uart
        self.response = FingerSensor.RESP_OK
        self.bufRx = bytearray(300)
        
    # Obtem ultima resposta
    def lastResponse(self):
        return self.response
    
    # Envia comando ao sensor
    def sendCmd(self, cmd):
        header = bytearray(9)
        checksum = bytearray(2)
        header[0] = 0xEF
        header[1] = 0x01
        header[2] = 0xFF
        header[3] = 0xFF
        header[4] = 0xFF
        header[5] = 0xFF
        header[6] = 0x01
        header[7] = (len(cmd)+2) >> 8
        header[8] = (len(cmd)+2) & 0xFF
        chk = header[6]+header[7]+header[8]
        for c in cmd:
            chk += c
        checksum[0] = (chk >> 8) & 0xFF
        checksum[1] = chk & 0xFF
        self.uart.write(header)
        self.uart.write(cmd)
        self.uart.write(checksum)
        
    # Lê resposta do sensor
    def getResponse(self):
        pos = 0
        state = 'S1'
        ok = False
        timeout = time()+10
        while True:
            rx = self.uart.read(1)
            if rx == None:
                if time() > timeout:
                    return False
                continue
            c = rx[0]
            self.bufRx[pos] = c
            pos = pos+1
            if state == 'S1':
                if c == 0xEF:
                    state = 'S2'
                else:
                    pos = 0
            elif state == 'S2':
                if c == 0x01:
                    state = 'ADDR'
                else:
                    pos = 0
                    state = 'S1'
            elif state == 'ADDR':
                if pos == 6:
                    state = 'TAG'
            elif state == 'TAG':
                chk = c
                state = 'LEN1'
            elif state == 'LEN1':
                chk += c
                dsize = c << 8
                state = 'LEN2'
            elif state == 'LEN2':
                chk += c
                i = dsize = dsize + c - 2
                state = 'DATA'
            elif state == 'DATA':
                chk = (chk + c) & 0xFFFF
                i = i -1
                if i == 0:
                    state = 'CHK1'
            elif state == 'CHK1':
                ok = c == (chk >> 8)
                state = 'CHK2'
            elif state == 'CHK2':
                ok = ok and (c == (chk & 0xFF))
                if ok:
                    self.response = self.bufRx[9]
                return ok
            
    # Apaga digitais armazenadas
    def clear(self):
        self.sendCmd(bytearray([FingerSensor.CMD_EMPTY]))
        ok = self.getResponse()
        return ok and self.response == FingerSensor.RESP_OK
    
    # Captura a imagem de uma digital
    def capture(self):
        self.sendCmd(bytearray([FingerSensor.CMD_GENIMG]))
        ok = self.getResponse()
        return ok and self.response == FingerSensor.RESP_OK
    
    # Converte a imagem atual de digital em uma feature
    # e salva em CharBuffer[numBuf] (1 or 2)
    def createFeature(self, numBuf):
        self.sendCmd(bytearray([FingerSensor.CMD_IMG2TZ, numBuf]))
        ok = self.getResponse()
        return ok and self.response == FingerSensor.RESP_OK
    
    # Cria um modelo (template) a partir das duas features em CharBuffer
    def createModel(self):
        self.sendCmd(bytearray([FingerSensor.CMD_REGMODEL]))
        ok = self.getResponse()
        return ok and self.response == FingerSensor.RESP_OK
        
    # Salva o modelo que esta em CharBuffer[numBuf] (1 or 2)
    # na posicao pos (0 to 149)
    def store(self, numbuf, pos):
        self.sendCmd(bytearray(
            [FingerSensor.CMD_STORE, numbuf, pos >> 8, pos & 0xFF]))
        ok = self.getResponse()
        return ok and self.response == FingerSensor.RESP_OK

    # Retorna quantas digitais estao armazenadas
    # -1 se algo der errado
    def count(self):
        self.sendCmd(bytearray([FingerSensor.CMD_TEMPLATECOUNT]))
        ok = self.getResponse()
        if ok and self.response == FingerSensor.RESP_OK:
            return (self.bufRx[10] << 8) + self.bufRx[11]
        return -1

    # Procura a digital correspondente a feature em CharBuffer[numBuf]
    # Se achar retorna posicao, senao retorna -1
    def search(self, numbuf):
        qty = self.count()
        cmd = [FingerSensor.CMD_SEARCH, numbuf, 0, 0, qty >> 8, qty & 0xFF]
        self.sendCmd(bytearray(cmd))
        ok = self.getResponse()
        if ok and self.response == FingerSensor.RESP_OK:
            return (self.bufRx[10] << 8) + self.bufRx[11]
        return -1
    