# Exemplo de LED como sensor de luz

import analogio
import digitalio
import board
from time import sleep

sensor = analogio.AnalogIn(board.A0)

def val():
    soma = 0
    for i in range(20):
        soma = soma + sensor.value;
    return soma//20

while True:
    print (val() // 100)
    sleep(2)
  
    

