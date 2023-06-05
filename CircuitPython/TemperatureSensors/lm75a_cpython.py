# Exemplo com LM75A

import board
from busio import I2C
from time import sleep

# Endereco I2C do LM75A
LM75_ADDR = 0x48

# Registradores do LM75A
LM75_TEMP = 0  
LM75_CONF = 1  
LM75_THYST = 2  
LM75_TOS = 3

i2c = I2C(sda=board.GP16, scl=board.GP17)

# Codifica temperatura para o sensor
def encodeTemp(temp):
    if temp >= 0:
        return int(temp / 0.5) << 7
    else:
        return (512 + int(temp/0.5)) << 7

# Decodifica temperatura informada pelo sensor
def decodeTemp(val):
    val = val >> 5
    if val >= 1024:
        return (val-2048)*0.125
    else:
        return val*0.125

# Escreve valor de 8 bits em um registrador
def writeReg8(reg, val):
    data = bytearray([reg, val & 0xFF])
    i2c.try_lock()
    i2c.writeto(LM75_ADDR, data)
    i2c.unlock()

# Escreve valor de 16 bits em um registrador
def writeReg16(reg, val):
    data = bytearray([reg, val >> 8, val & 0xFF])
    i2c.try_lock()
    i2c.writeto(LM75_ADDR, data)
    i2c.unlock()

# Le valor de 16 bits de um registrador
def readReg16(reg):
    selreg = bytearray([reg])
    val = bytearray([0,0])
    i2c.try_lock()
    i2c.writeto_then_readfrom(LM75_ADDR, selreg, val)
    i2c.unlock()
    return val

# Configura o sensor e define limites para a saida OS
writeReg8(LM75_CONF,0)
writeReg16(LM75_TOS,encodeTemp(22.5))
writeReg16(LM75_THYST,encodeTemp(20.0))

# Laco principal: le a temperatura
while True:
    sleep(0.5)
    data = readReg16(LM75_TEMP)
    temp = decodeTemp((data[0] << 8) + data[1])
    print ('Temperatura = {}C'.format (temp))
