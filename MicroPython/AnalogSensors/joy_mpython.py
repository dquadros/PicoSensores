# Exemplo com Joystick  Analogico

from machine import Pin, ADC, PWM
from time import sleep

# Potenciometros
horiz = ADC(0)
vert = ADC(1)

# LEDs, frequencia do PWM e 1KHz
def initPWM(pin):
    pwm = PWM(Pin(pin, Pin.OUT))
    pwm.freq(1000)
    pwm.duty_u16(0)
    return pwm
    
ledRight = initPWM(12)
ledUp = initPWM(13)
ledDown = initPWM(14)
ledLeft = initPWM(15)

# Ignorar variacoes de ate
ZONA_MORTA = 3000

# Laco Principal
while True:
    # amostra potenciometros
    h = horiz.read_u16()
    v = vert.read_u16()
    
    # eixo horizontal
    if h > (32767+ZONA_MORTA):
        ledRight.duty_u16(0)
        ledLeft.duty_u16(h-32768)
    elif h <= (32767-ZONA_MORTA):
        ledRight.duty_u16(32767-h)
        ledLeft.duty_u16(0)
    else:
        ledRight.duty_u16(0)
        ledLeft.duty_u16(0)

    # eixo vertical
    if v > (32767+ZONA_MORTA):
        ledDown.duty_u16(v-32768)
        ledUp.duty_u16(0)
    elif v <= (32767-ZONA_MORTA):
        ledDown.duty_u16(0)
        ledUp.duty_u16(32767-v)
    else:
        ledDown.duty_u16(0)
        ledUp.duty_u16(0)
