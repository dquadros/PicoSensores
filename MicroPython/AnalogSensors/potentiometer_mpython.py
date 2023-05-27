#  Exemplo com Potenciometro

from machine import Pin, ADC, PWM
from time import sleep

# Potenciometro no ADC0
sensor = ADC(0)

# LED da placa, frequencia do PWM e 1KHz
led = PWM(Pin(25, Pin.OUT))
led.freq(1000)
led.duty_u16(0)
              
# Laco principal
while True:
    # le posicao do potenciometro
    val = sensor.read_u16()
    print(val)
    # ajusta intensidade do LED
    led.duty_u16(val)
    # dorme entre leituras
    sleep(0.2)
