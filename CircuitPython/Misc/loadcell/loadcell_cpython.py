# Exemplo com Célula de Carga

import board
import digitalio
from busio import I2C
from i2c_lcd import lcd_pcf8574
from hx711.hx711_pio import HX711_PIO
from time import sleep

# Inicia botão
button = digitalio.DigitalInOut(board.GP15)
button.pull = digitalio.Pull.UP

# Inicia o display
i2c = I2C(sda=board.GP16, scl=board.GP17)
lcd = lcd_pcf8574(i2c)
lcd.init()
lcd.backlightOn()

# Mostra mensagem e espera botão
def keyPress(msg):
    lcd.displayClear()
    lcd.displayWrite(0,0,msg)
    lcd.displayWrite(1,0,'Aperte o botao')
    while button.value:
        sleep(0.1)
    sleep(0.1) # debounce
    lcd.displayWrite(1,0,'Solte o botao ')
    while not button.value:
        sleep(0.1)
    sleep(0.1) # debounce
    lcd.displayWrite(1,0,'Aguarde        ')
    
# Calibracao inicial
sensor = HX711_PIO(board.GP12, board.GP13)
keyPress('Balanca vazia')
sensor.tare()
keyPress('Coloque 1kg')
sensor.determine_scalar(1.0)
if sensor.read() < 0:
    sensor.scalar = -sensor.scalar

# Laco principal
lcd.displayClear()
lcd.displayWrite(0,0,'Balanca Pronta')
while True:
    weight = sensor.read()
    lcd.displayWrite(1,0,'{:7.3f}kg'.format(weight))
    sleep(0.5)
