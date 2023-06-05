# Exemplo com Sensor de Temperatura HDC1080

from machine import I2C,Pin
from time import sleep

# Endereco I2C do HDC1080
HDC_ADDR = 0x40

# Registradores do HDC1080
HDC_TEMP = 0
HDC_HUM = 1
HDC_CONF = 2
HDC_MANID = 0xFE
HDC_DEVID = 0xFF  

i2c = I2C(0, sda=Pin(16), scl=Pin(17))

# Le registrador de 16 bits
def readReg16(reg):
    data = i2c.readfrom_mem(HDC_ADDR,reg,2)
    return (data[0] << 8) + data[1]

# Verifica IDs de Fabricante e Dispositivo
print ('Fabricante: {:04X}'.format(readReg16(HDC_MANID)))
print ('Dispositivo: {:04X}'.format(readReg16(HDC_DEVID)))

# Laco Principal
while True:
    i2c.writeto(HDC_ADDR, bytearray([HDC_TEMP]))
    sleep (0.02)
    data = i2c.readfrom(HDC_ADDR, 4)
    r = (data[0] << 8) + data[1]
    temp = r*165/65536 - 40.0
    r = (data[2] << 8) + data[3]
    humid = r*100/65536
    print ('Temperatura: {:.1f}C Umidade: {:.0f}%'.format(temp, humid))
    sleep(2)

