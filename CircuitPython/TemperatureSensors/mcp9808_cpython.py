# Exemplo com Sensor MCP9808

import board
from busio import I2C
from time import sleep

# Endereco I2C do  MCP9808
MCP9808_ADDR = 0x18

# Registradores do MCP9808
MCP9808_CONFIG = 1
MCP9808_UPPER = 2  
MCP9808_LOWER = 3
MCP9808_CRIT = 4
MCP9808_TA = 5
MCP9808_MANID = 6
MCP9808_DEVID = 7
MCP9808_RESOL = 8

# Codifica temperatura para o sensor
def encodeTemp(temp):
    if temp >= 0:
        val =  int(temp/0.25) << 2
    else:
        val = (2048 + int(temp/0.25)) << 2
    return val

# Decodifica temperatura lida do sensor
def decodeTemp(val):
    sign = val & 0x1000
    val = val & 0x1FFF
    if sign != 0:
        val = val - 0x2000
    return val/16

i2c = I2C(sda=board.GP16, scl=board.GP17)

# Escreve valor de 16 bits em um registrador
def writeReg16(reg, val):
    data = bytearray([reg,val >> 8, val & 0xFF])
    i2c.try_lock()
    i2c.writeto(MCP9808_ADDR, data)
    i2c.unlock()

# Le registrador de 16 bits
def readReg16(reg):
    selreg = bytearray([reg])
    val = bytearray([0,0])
    i2c.try_lock()
    i2c.writeto_then_readfrom(MCP9808_ADDR, selreg, val)
    i2c.unlock()
    return (val[0] << 8) + val[1]

# Verifica IDs de fabricante e dispositivo
print ('Fabricante: {:04X}'.format(readReg16(MCP9808_MANID)))
devID = readReg16(MCP9808_DEVID);
print ('Dispositivo: {:02X} rev {}'.format(devID >>8, devID&0xFF))

# Define limites para saida Alert
writeReg16(MCP9808_CRIT,encodeTemp(30.0))
writeReg16(MCP9808_UPPER,encodeTemp(23.0))
writeReg16(MCP9808_LOWER,encodeTemp(20.0))
writeReg16(MCP9808_CONFIG,0x0008)

#Laco principal: le temperatura
while True:
    sleep(0.5)
    print ('Temperatura = {}C'.format(
        decodeTemp(readReg16(MCP9808_TA))))
