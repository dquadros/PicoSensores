# Exemplo com Fototransistor

from machine import Pin, ADC

# Fototransistor ligado ao ADC0
sensor = ADC(0)

val_ant = 0

# Laco Principal
while True:
    val = sensor.read_u16()
    dif = abs(val - val_ant)
    if dif > 5000:
        print(val)
        val_ant = val
