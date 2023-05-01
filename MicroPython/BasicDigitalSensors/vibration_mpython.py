# Exemplo com Sensor de Vibracao

from machine import Pin
import time

sensor = Pin(16, Pin.IN)
led = Pin(25, Pin.OUT)
led.off()
vibr = []
count = 0
while True:
    time.sleep_ms(10)
    if len(vibr) >= 100:
        # despreza o mais antigo
        count = count - vibr.pop()
    vibr.insert(0, sensor.value())
    count = count + vibr[0]
    if count > 80:
        led.on()
    else:
        led.off()
