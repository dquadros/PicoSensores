# Exemplo com Célula de Carga

import rp2 
from rp2 import PIO, asm_pio
from machine import Pin, I2C
from i2c_lcd import lcd_pcf8574
from time import sleep

# Programa da PIO
@asm_pio(set_init=rp2.PIO.OUT_LOW,autopush=True,
         fifo_join=PIO.JOIN_RX)
def HX711_PIO():
    # espera uma nova conversao
    wait (1,pin,0)
    wait (0,pin,0)
    
    # lê os bits de dados
    set (x,23)
    label('readbit')
    set (pins, 1) [3]
    in_ (pins, 1)
    set (pins, 0) [3]
    jmp (x_dec, 'readbit')
    
    # acrescenta um pulso para selecionar ganho de 128
    set (pins,1) [4]
    set (pins,0)

# Configura os pinos
pin_data = Pin(12, Pin.IN)
pin_clock = Pin(13, Pin.OUT)

# Inicia a PIO
sm = rp2.StateMachine(0)
sm.init(HX711_PIO,
        set_base=pin_clock,
        in_base=pin_data,
        in_shiftdir=PIO.SHIFT_LEFT,
        freq=5_000_000,
        push_thresh=24)
sm.active(1)

# Limpa a fila da PIO
def clearFIFO():
    while sm.rx_fifo() > 0:
        sm.get()

# Lê um valor bruto do HX711
def hx711_raw():
    val = sm.get()
    if (val & 0x800000) != 0:
        val -= 0x100000
    return val

# Lê uma média
def hx711_avg(count = 10):
    sum = 0
    clearFIFO()
    for _ in range(count):
        sum += hx711_raw()
    return sum // count

# Inicia o botão
button = Pin (15, Pin.IN, Pin.PULL_UP)

# Inicia o display
i2c = I2C(0, sda=Pin(16), scl=Pin(17))
lcd = lcd_pcf8574(i2c)
lcd.init()
lcd.backlightOn()

# Mostra mensagem e espera o botão
def keyPress(msg):
    lcd.displayClear()
    lcd.displayWrite(0,0,msg)
    lcd.displayWrite(1,0,'Aperte o botao')
    while button.value():
        sleep(0.1)
    sleep(0.1) # debounce
    lcd.displayWrite(1,0,'Solte o botao ')
    while not button.value():
        sleep(0.1)
    sleep(0.1) # debounce
    lcd.displayWrite(1,0,'Espere        ')
    
# Calibração inicial
keyPress('Balanca vazia')
tare = hx711_avg(50)
keyPress('Coloque 1kg')
scale = (hx711_avg(50) - tare)/1.0

# Laço principal
lcd.displayClear()
lcd.displayWrite(0,0,'Balanca pronta')
while True:
    weight = (hx711_avg() - tare)/scale
    lcd.displayWrite(1,0,'{:7.3f}kg'.format(weight))
    sleep(0.5)
