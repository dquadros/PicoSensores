# Exemplo com Fototransistor

import analogio
import digitalio
import board

# Fototransistor ligado ao ADC0
sensor = analogio.AnalogIn(board.A0)

val_ant = 0

# Laco Principal
while True:
    val = sensor.value
    dif = abs(val - val_ant)
    if dif > 5000:
        print(val)
        val_ant = val

