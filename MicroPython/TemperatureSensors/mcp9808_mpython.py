#Exemplo com Sensor MCP9808

from machine import I2C,Pin
from time import sleep

# Endereco I2C do  MCP9808
MCP9808_ADDR = 0x18

#Registradores do MCP9808
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
    return bytearray([val >> 8, val & 0xFF])

# Decodifica temperatura lida do sensor
def decodeTemp(val):
    sign = val & 0x1000
    val = val & 0x1FFF
    if sign != 0:
        val = val - 0x2000
    return val/16

i2c = I2C(0, sda=Pin(16), scl=Pin(17))

# Le registrador de 16 bits
def readReg16(reg):
    data = i2c.readfrom_mem(MCP9808_ADDR,reg,2)
    return (data[0] << 8) + data[1]

# Verifica IDs de fabricante e dispositivo
print ('Fabricante: {:04X}'.format(readReg16(MCP9808_MANID)))
devID = readReg16(MCP9808_DEVID);
print ('Dispositivo: {:02X} rev {}'.format(devID >>8, devID&0xFF))

# Define limites para saida Alert
i2c.writeto_mem(MCP9808_ADDR,MCP9808_CRIT,encodeTemp(30.0))
i2c.writeto_mem(MCP9808_ADDR,MCP9808_UPPER,encodeTemp(23.0))
i2c.writeto_mem(MCP9808_ADDR,MCP9808_LOWER,encodeTemp(20.0))
i2c.writeto_mem(MCP9808_ADDR,MCP9808_CONFIG,b'\x00\x08')

# Laco principal: le temperatura
while True:
    sleep(0.5)
    print ('Temperatura = {}C'.format(
        decodeTemp(readReg16(MCP9808_TA))))