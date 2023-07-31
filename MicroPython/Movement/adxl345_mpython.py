# Exemplo sensor ADXL345

from machine import I2C, Pin
from time import sleep

# Classe para acessar o sensor
class ADXL345:
    
    _DEFAULT_ADDRESS = 0x1D
    _ALT_ADDRESS = 0x53
    _DEVICE_ID = 0xE5
    
    _DEVID = 0x00
    _POWER_CTL = 0x2D
    _DATA_FORMAT = 0x31
    _DATAX0 = 0x32

    # Construtor
    def __init__(self, i2c, addr):
        self.i2c = i2c
        self.addr = addr
    
    # Configura o sensor
    def begin(self):
        self.id = self.read_reg(ADXL345._DEVID)
        if self.id != ADXL345._DEVICE_ID:
            print ('WARNING: Wrong ID! {:02X}'.format(self.id))
        i2c.writeto_mem(self.addr, ADXL345._POWER_CTL, b'\x08')
        i2c.writeto_mem(self.addr, ADXL345._DATA_FORMAT, b'\x00')

    # ID do sensor
    def get_id(self):
        return self.id
    
    # Lê valores brutos
    # accX, acccY, accZ, temp, gyroX, gyroY, gyroZ
    def read_raw(self):
        data = i2c.readfrom_mem(self.addr, ADXL345._DATAX0, 6)
        raw = []
        for i in range (0, 6, 2):
            v = (data[i+1] << 8) + data[i]
            if (v & 0x8000) != 0:
                v = v - 0x10000
            raw.append(v)
        return raw
    
    # Lê aceleração em g
    def get_accel(self, scale = 256.0):
        data = self.read_raw()
        return (data[0]/scale, data[1]/scale, data[2]/scale)

    # Lê um registrador de 8 bits
    def read_reg(self, reg):
        return i2c.readfrom_mem(self.addr, reg, 1)[0]

# Programa de teste
i2c = I2C(0, sda=Pin(16), scl=Pin(17))
sensor = ADXL345(i2c, ADXL345._ALT_ADDRESS)
sensor.begin()
print('ID = {:02X}'.format(sensor.get_id()))
while True:
    print (sensor.read_raw())
    x,y,z = sensor.get_accel()
    print ('Aceleracao X:{:.1f}g y:{:.1f}g Z:{:.1f}g'.format(x, y, z))
    print ()
    sleep(2)

