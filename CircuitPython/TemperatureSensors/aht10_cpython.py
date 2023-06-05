# Exemplo com Sensor de Temperatura AHT10

import board
from digitalio import DigitalInOut, Pull
from busio import I2C
from time import sleep

# Endereco I2C do AHT10
AHT_ADDR = 0x38

i2c = I2C(sda=board.GP16, scl=board.GP17)

# Le status do ADH10
def getStatus():
    resp = bytearray([0])
    i2c.try_lock()
    i2c.readfrom_into (AHT_ADDR, resp)
    i2c.unlock()
    return resp[0]

# Verifica se precisa calibrar
status = getStatus()
if (status & 0x08) == 0:
    print ('Calibrando')
    i2c.try_lock()
    i2c.writeto(AHT_ADDR, b'\xE1\x08\x00')
    i2c.unlock()
    sleep(0.01)

# Laco Principal
while True:
    # dispara leitura
    i2c.try_lock()
    i2c.writeto(AHT_ADDR, b'\xAC\x33\x00')
    i2c.unlock()
    # aguarda leitura
    sleep(0.08)
    # le resposta
    resp = bytearray([0,0,0,0,0,0])
    i2c.try_lock()
    i2c.readfrom_into (AHT_ADDR, resp)
    i2c.unlock()
    # decodifica resposta
    umid = (resp[1] << 12) + (resp[2] << 4) + (resp[3] >> 4)
    umid = (umid / 0x100000) * 100.0
    temp = ((resp[3] & 0x0F) << 16) + (resp[4] << 8) + resp[5]
    temp = (temp / 0x100000) * 200.0 - 50.0
    # mostra resultado
    print ('Umidade = {:.1f}%  Temperatura = {:.1f}C'.format(umid, temp))
    # da um tempo entre leituras
    sleep(2)
