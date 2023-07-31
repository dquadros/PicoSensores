# Exemplo com HC-SR04

import utime
import rp2 
from rp2 import PIO, asm_pio
from machine import Pin

# Programa PIO
@asm_pio(set_init=rp2.PIO.OUT_LOW,autopush=False)
def ULTRA_PIO():
    # espera solicitação e salva o timeout
    pull()
    mov (x,osr)
    
    # envia pulso de 10 us (20 ciclos)
    set(pins,1) [19]
    set(pins,0)
    
    # espera o início do pulso de eco
    wait(1,pin,0)
    
    # espera o fim do pulso de eco
    # decrementa X a cada 2 ciclos (1us)
    label('dowait')
    jmp(pin,'continue')
    jmp('end')    
    label('continue')
    jmp(x_dec,'dowait')
    
    # retorna duracao do pulso
    label('end')
    mov(isr,x)
    push()

# Configura pinos
trigger = Pin(17, Pin.OUT)
echo = Pin(16, Pin.IN)
sm = rp2.StateMachine(0)

# Faz medições
while True:
    sm.init(ULTRA_PIO,freq=2000000,set_base=trigger,in_base=echo,jmp_pin=echo)
    sm.active(1)
    sm.put(300000)
    val = sm.get()
    sm.active(0)
    if val < 300000:
        ellapsed = 300000 - val
        distance = (ellapsed * 0.0343) / 2
        print('Distancia = {0:.1f} cm'.format(distance))
        utime.sleep_ms(1000)
    else:
        print('Timeout')
  