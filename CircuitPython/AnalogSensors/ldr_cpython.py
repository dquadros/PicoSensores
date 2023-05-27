# Exemplo com LDR

import analogio
import digitalio
import board
from time import sleep
import random

# LDR esta no ADC0
sensor = analogio.AnalogIn(board.A0)

# Buzzer no pino 15
buzzer = digitalio.DigitalInOut(board.GP15)
buzzer.direction = digitalio.Direction.OUTPUT
buzzer.value = False

# Limete claro/escuro
DARK = 5000

# Laco Principal
while True:
    # Dorme por 3 a 60 segundos
    sleep(random.uniform(3, 60))
    # le LDR e verifica se esta escuro
    val = sensor.value
    if val < DARK:
        # bipe curto
        buzzer.value = True
        sleep(0.1)
        buzzer.value = False
