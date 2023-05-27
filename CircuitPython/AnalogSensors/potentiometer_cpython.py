# Exemplo com Potenciometro

import analogio
import pwmio
import board
from time import sleep

# Potenciometro no ADC0
sensor = analogio.AnalogIn(board.A0)

# LED da placa, frequencia do PWM e 1KHz
led = pwmio.PWMOut(board.LED, frequency=1000, duty_cycle=0)
              
# Laco Principal
while True:
    # le posicao do potenciomentro
    val = sensor.value
    print(val)
    # acerta intensidade do LED
    led.duty_cycle=val
    # dorme entre leituras
    sleep(0.2)
