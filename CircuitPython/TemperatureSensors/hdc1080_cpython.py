# Exemplo com Sensor de Temperatura HDC1080

import board
from digitalio import DigitalInOut, Pull
from busio import I2C
from time import sleep

#Endereco I2C do HDC1080
HDC_ADDR = 0x40

#Registradores do HDC1080
HDC_TEMP = 0
HDC_HUM = 1
HDC_CONF = 2
HDC_MANID = 0xFE
HDC_DEVID = 0xFF  

i2c = I2C(sda=board.GP16, scl=board.GP17)

# Le registrador de 16 bits
def readReg16(reg):
    selreg = bytearray([reg])
    val = bytearray([0,0])
    i2c.try_lock()
    i2c.writeto_then_readfrom(HDC_ADDR, selreg, val)
    i2c.unlock()
    return (val[0]<<8)+val[1]

# Verifica IDs de Fabricante e Dispositivo
print ('Fabricante: {:04X}'.format(readReg16(HDC_MANID)))
print ('Dispositivo: {:04X}'.format(readReg16(HDC_DEVID)))

# Laco Principal
while True:
    i2c.try_lock()
    i2c.writeto(HDC_ADDR, bytearray([HDC_TEMP]))
    i2c.unlock()
    sleep (0.02)
    i2c.try_lock()
    data = bytearray([0,0,0,0])
    i2c.readfrom_into(HDC_ADDR, data)
    r = (data[0] << 8) + data[1]
    temp = r*165/65536 - 40.0
    r = (data[2] << 8) + data[3]
    humid = r*100/65536
    print ('Temperatura: {:.1f}C Umidade: {:.0f}%'.format(temp, humid))
    sleep(2)

