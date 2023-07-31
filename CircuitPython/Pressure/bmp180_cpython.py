# Exemplo com Sensor BMP085/BMP180
import board
from busio import I2C
from time import sleep
import bmp180

i2c = I2C(sda=board.GP16, scl=board.GP17)
bmp = bmp180.BMP180(i2c)

while True:
    press = bmp.pressure
    temp = bmp.temperature
    alt = bmp.altitude
    print ('Temperatura: {:.1f} C  Pressao: {:.0f} Pa  {:.1f} m'.format(
        temp, press, alt))
    sleep(2)