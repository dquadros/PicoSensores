# Exemplo com LM75A

from machine import I2C,Pin
from time import sleep

# Endereco I2C do LM75A
LM75_ADDR = 0x48

# Registradores do LM75A
LM75_TEMP = 0  
LM75_CONF = 1  
LM75_THYST = 2  
LM75_TOS = 3

# Codifica temperatura para o sensor
def encodeTemp(temp):
    if temp >= 0:
        val =  int(temp / 0.5) << 7
    else:
        val = (512 + int(temp/0.5)) << 7
    return bytearray([val >> 8, val & 0xFF])

# Decodifica temperatura informada pelo sensor
def decodeTemp(val):
    val = val >> 5
    if val >= 1024:
        return (val-2048)*0.125
    else:
        return val*0.125

i2c = I2C(0, sda=Pin(16), scl=Pin(17))

# Configura o sensor e define limites para a saida OS
i2c.writeto_mem(LM75_ADDR,LM75_CONF,b'\x00')
i2c.writeto_mem(LM75_ADDR,LM75_TOS,encodeTemp(22.5))
i2c.writeto_mem(LM75_ADDR,LM75_THYST,encodeTemp(20.0))

# Laco principal: le a temperatura
while True:
    sleep(0.5)
    data = i2c.readfrom_mem(LM75_ADDR,LM75_TEMP,2)
    temp = decodeTemp((data[0] << 8) + data[1])
    print ('Temperatura = {}C'.format (temp))
