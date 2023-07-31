# Exemplo com iButton

from machine import Pin
import onewire
from time import sleep

# Inicia a rede OnewWire
ow = onewire.OneWire(Pin(16))

# Laco Principal
last = None
while True:
    if ow.reset():
        # Obteve pulso de persença, procura dispositivos
        sensors = ow.scan()
        # Confere que é um iButton
        if (len(sensors) > 0) and (sensors[0] != last) and \
          (ow.crc8(sensors[0]) == 0) and (sensors[0][0] == 1):
            # Extrai e mostra ID
            last = sensors[0]
            id = ''
            for i in range (6,0,-1):
                id = id + '{:02X}'.format(last[i])
            print('ID = '+id)
            sleep(2)
    else:
        last = None
        sleep(.5)
