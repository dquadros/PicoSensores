#Exemplo com Sensor Efeito Hall Analogico

from machine import Pin, ADC
from time import sleep

# Sensor esta em ADC0
sensor = ADC(Pin(26))

# Pegar uma media de 50 leituras
def val():
  sum = 0
  for i in range(50):
    sum = sum + sensor.read_u16();
  return sum // 50

# Assume que nao tem campo magnetico no inicio
zero = val() 
print('Zero = {}'.format(zero))
sleep(1)
print ('Pronto')

# Laco Principal
while True:
  field = val()
  if abs(field-zero) < 100:
      print ('Sem campo')
  elif field > zero:
    print ('Campo = S {}'.format (field-zero))
  else:
    print ('Campo = N {}'.format (zero- field))
  sleep(1)
