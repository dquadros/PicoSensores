# Exemplo com ADXL345
import board
from digitalio import DigitalInOut, Pull
from busio import I2C
from time import sleep
import adafruit_adxl34x

# Configura sensor
i2c = I2C(sda=board.GP16, scl=board.GP17)
sensor = adafruit_adxl34x.ADXL345(i2c)
sensor.enable_motion_detection()
sensor.enable_tap_detection(threshold = 100)
sensor.enable_freefall_detection()

# Loop Principal: mostra eventos
while True:
    sleep(0.1)
    if sensor.events['tap']:
        print('BATIDA', sensor.acceleration)
    if sensor.events['motion']:
        print('MOVIMENTO', sensor.acceleration)
    if sensor.events['freefall']:
        print('QUEDA LIVRE', sensor.acceleration)
