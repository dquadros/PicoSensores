# Exemplo Sensor de Temperatura DS18B20

import board
from adafruit_onewire.bus import OneWireBus
from adafruit_ds18x20 import DS18X20
from time import sleep

# Identifica dispositivos na via OneWire
ow_bus = OneWireBus(board.GP16)
devices = ow_bus.scan()
sensors = []
for device in devices:
    print(''.join(hex(i)[2:4] for i in device.rom))
    if device.family_code == 0x28:
        sensors.append(DS18X20(ow_bus, device))
print()

# Laco Principal
while True:
    for sensor in sensors:
        sensor.start_temperature_read()
    sleep(1)
    temp = ''
    for sensor in sensors:
        temp = temp + '{:.2f}C '.format(sensor.read_temperature())
    print (temp)

