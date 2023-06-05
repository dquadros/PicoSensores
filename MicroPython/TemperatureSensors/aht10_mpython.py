# Exemplo com Sensor de Temperatura AHT10

from machine import I2C,Pin
from time import sleep_ms

# Endereco I2C do AHT10
AHT_ADDR = 0x38

i2c = I2C(0, sda=Pin(16), scl=Pin(17))

# Le status do ADH10
def getStatus():
    resp = i2c.readfrom (AHT_ADDR, 1)
    return resp[0]

# Verifica se precisa calibrar
status = getStatus()
if (status & 0x08) == 0:
    print ('Calibrando')
    i2c.writeto(AHT_ADDR, b'\xE1\x08\x00')
    sleep_ms(10)

# Laco Principal
while True:
    # dispara leitura
    i2c.writeto(AHT_ADDR, b'\xAC\x33\x00')
    # aguarda leitura
    sleep_ms(80)
    # le resposta
    resp = i2c.readfrom (AHT_ADDR, 6)
    # decodifica resposta
    umid = (resp[1] << 12) + (resp[2] << 4) + (resp[3] >> 4)
    umid = (umid / 0x100000) * 100.0
    temp = ((resp[3] & 0x0F) << 16) + (resp[4] << 8) + resp[5]
    temp = (temp / 0x100000) * 200.0 - 50.0
    # mostra resultado
    print ('Umidade = {:.1f}%  Temperatura = {:.1f}C'.format(umid, temp))
    # da um tempo entre leituras
    sleep_ms(2000)
