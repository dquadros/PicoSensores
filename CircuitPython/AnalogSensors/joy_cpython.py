# Exemplo com Joystick  Analogico

import analogio
import pwmio
import board

# Potenciometros
horiz = analogio.AnalogIn(board.A0)
vert = analogio.AnalogIn(board.A1)

# LEDs, frequencia do PWM e 1KHz
ledRight = pwmio.PWMOut(board.GP12, frequency=1000, duty_cycle=0)
ledUp = pwmio.PWMOut(board.GP13, frequency=1000, duty_cycle=0)
ledDown = pwmio.PWMOut(board.GP14, frequency=1000, duty_cycle=0)
ledLeft = pwmio.PWMOut(board.GP15, frequency=1000, duty_cycle=0)

# Ignorar variacoes de ate
ZONA_MORTA = 3000

# Laco principal
while True:
    # amostra potenciometros
    h = horiz.value
    v = vert.value
    
    # eixo horizontal
    if h > (32767+ZONA_MORTA):
        ledRight.duty_cycle = 0
        ledLeft.duty_cycle = h-32768
    elif h <= (32767-ZONA_MORTA):
        ledRight.duty_cycle = 32767-h
        ledLeft.duty_cycle = 0
    else:
        ledRight.duty_cycle = 0
        ledLeft.duty_cycle = 0

    # eixo vertical
    if v > (32767+ZONA_MORTA):
        ledDown.duty_cycle = v-32768
        ledUp.duty_cycle = 0
    elif v <= (32767-ZONA_MORTA):
        ledDown.duty_cycle = 0
        ledUp.duty_cycle = 32767-v
    else:
        ledDown.duty_cycle = 0
        ledUp.duty_cycle = 0
