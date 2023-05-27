# Exemplo com Sensor Efeito Hall Analogico

import analogio
import board
from time import sleep

# Sensor esta em ADC0
sensor = analogio.AnalogIn(board.A0)

# Pegar uma media de 50 leituras
def val():
  sum = 0
  for i in range(50):
    sum = sum + sensor.value;
  return sum // 50

# Assume que nao tem campo magnetico no inicio
zero = val() 
print('Zero = {}'.format(zero))
sleep(1)
print ('Pronto')

# Main loop
while True:
  field = val()
  if abs(field-zero) < 100:
      print ('Sem campo')
  elif field > zero:
    print ('Campo = S {}'.format (field-zero))
  else:
    print ('Campo = N {}'.format (zero- field))
  sleep(1)

