# Exemplo com sensor MPU6050
from machine import I2C, Pin
from time import sleep

# Classe para acessar o sensor
class MPU6050:
    
    _DEFAULT_ADDRESS = 0x68  # endereço I2C padrão c/ AD0 baixo
    _DEVICE_ID = 0x68  # identificação do dispositivo

    _SMPLRT_DIV = 0x19  # sample rate divisor register
    _CONFIG = 0x1A  # General configuration register
    _GYRO_CONFIG = 0x1B  # Gyro specfic configuration register
    _ACCEL_CONFIG = 0x1C  # Accelerometer specific configration register
    _ACCEL_OUT = 0x3B  # base address for sensor data reads
    _TEMP_OUT = 0x41  # Temperature data high byte register
    _GYRO_OUT = 0x43  # base address for sensor data reads
    _SIG_PATH_RESET = 0x68  # register to reset sensor signal paths
    _PWR_MGMT_1 = 0x6B  # Primary power/sleep control register
    _PWR_MGMT_2 = 0x6C  # Secondary power/sleep control register
    _WHO_AM_I = 0x75  # Device ID register
    
    # Construtor
    def __init__(self, i2c, addr):
        self.i2c = i2c
        self.addr = addr
    
    # Configura o sensor
    def begin(self):
        self.reset()
        # Confere o ID
        self.id = self.read_reg(MPU6050._WHO_AM_I)
        if self.id != MPU6050._DEVICE_ID:
            print ('AVISO: ID incorreto! {:02X}'.format(self.id))
        # Configura
        # Bandwith 260Hz (Accel) & 256Hz (Gyro)
        # Range ±500°/s (Gyro) ±2g (Accel)
        i2c.writeto_mem(self.addr, MPU6050._CONFIG, b'\x00')
        sleep(0.05)
        i2c.writeto_mem(self.addr, MPU6050._ACCEL_CONFIG, b'\x00')
        i2c.writeto_mem(self.addr, MPU6050._GYRO_CONFIG, b'\x08')
        i2c.writeto_mem(self.addr, MPU6050._SMPLRT_DIV, b'\x00')
        i2c.writeto_mem(self.addr, MPU6050._PWR_MGMT_1, b'\x01')
        i2c.writeto_mem(self.addr, MPU6050._PWR_MGMT_2, b'\x00')
        sleep(0.02)
    
    # Identificação do sensor
    def get_id(self):
        return self.id
    
    # Reinicia o sensor
    def reset(self):
        val = bytearray([self.read_reg(MPU6050._PWR_MGMT_1) | 0x80])
        i2c.writeto_mem(self.addr, MPU6050._PWR_MGMT_1, val)
        while (val[0] & 0x80) != 0:
            val = i2c.readfrom_mem(self.addr, MPU6050._PWR_MGMT_1, 1)
            sleep(0.001)
        val = bytearray([self.read_reg(MPU6050._SIG_PATH_RESET) | 0x07])
        i2c.writeto_mem(self.addr, MPU6050._SIG_PATH_RESET, val)
        while (val[0] & 0x07) != 0:
            val = i2c.readfrom_mem(self.addr, MPU6050._SIG_PATH_RESET, 1)
            sleep(0.001)
    
    # Lê valores brutos
    # accX, acccY, accZ, temp, gyroX, gyroY, gyroZ
    def read_raw(self):
        data = i2c.readfrom_mem(self.addr, MPU6050._ACCEL_OUT, 14)
        raw = []
        for i in range (0, 14, 2):
            v = (data[i] << 8) + data[i+1]
            if (v & 0x8000) != 0:
                v = v - 0x10000
            raw.append(v)
        return raw
    
    # Retorna aceleração em g
    def get_accel(self, scale = 16384.0):
        data = self.read_raw()
        return (data[0]/scale, data[1]/scale, data[2]/scale)
    
    # Lê temperature em C
    def get_temp(self):
        data = self.read_raw()
        return data[3]/340.00 + 36.53
    
    # Lê giroscópio em °/s
    def get_gyro(self, scale = 65.5):
        data = self.read_raw()
        return (data[4]/scale, data[5]/scale, data[6]/scale)
    
    # Lê um registador de 8 bits
    def read_reg(self, reg):
        return i2c.readfrom_mem(self.addr, reg, 1)[0]
    

# Programa de teste
i2c = I2C(0, sda=Pin(16), scl=Pin(17))
sensor = MPU6050(i2c, MPU6050._DEFAULT_ADDRESS)
sensor.begin()
print('ID = {:02X}'.format(sensor.get_id()))
while True:
    print (sensor.read_raw())
    x,y,z = sensor.get_accel()
    print ('Aceleracao X:{:.1f}g y:{:.1f}g Z:{:.1f}g'.format(x, y, z))
    x,y,z = sensor.get_gyro()
    print ('Giroscopio X:{:.1f} y:{:.1f} Z:{:.1f}'.format(x,y,z))
    print ('Temperatura: {:.1f}C'.format(sensor.get_temp()))
    sleep(2)
