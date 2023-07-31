# Exemplo com HC-SR04

import digitalio
import pulseio
import board
from time import sleep
from time import monotonic

# Configura pino Trig
trigger = digitalio.DigitalInOut(board.GP17)
trigger.direction = digitalio.Direction.OUTPUT
trigger.value = False

# Configura pino Echo
echo = pulseio.PulseIn(board.GP16, idle_state=False)
echo.pause()

# Faz medições
while True:
    # prepara medição
    echo.clear()
    echo.resume()
    
    # pulsa o trigger
    trigger.value = True
    sleep(0.001)
    trigger.value = False
    
    # espera capturar o eco
    tout = monotonic() + 0.1
    while (len(echo) == 0) and (monotonic() < tout):
        pass
    echo.pause()

    # Calcula distancia
    if len(echo) > 0:
        distance = (echo[0] * 0.0343) / 2
        print('Distancia = {0:.1f} cm'.format(distance))
    else:
        print('Timeout')

    sleep(2)
