# Exemplo Sensor de Temperatura DS18B20

from machine import Pin
import onewire
import ds18x20
from time import sleep

# Identifica dispositivos na via OneWire
ow = onewire.OneWire(Pin(16))
ds = ds18x20.DS18X20(ow)
sensors = ds.scan()
for sensor in sensors:
    print(''.join(hex(i)[2:4] for i in sensor))
print()

# Laco Principal
while True:
    ds.convert_temp()
    sleep(1)
    temp = ''
    for sensor in sensors:
        temp = temp + '{:.2f}C '.format(ds.read_temp(sensor))
    print (temp)
