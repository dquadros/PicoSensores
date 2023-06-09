# Exemplo com Botao
from machine import Pin
import time

class Button:
    def __init__(self, pin, debounce=20):
        self.pinButton = Pin(pin, Pin.IN, Pin.PULL_UP)
        self.pressed = False
        self.debounce = debounce
        self.last = self.pinButton.value()
        self.lastTime = time.ticks_ms()
        
    def isPressed(self):
        val = self.pinButton.value()
        if val != self.last:
            # leitura mudou
            self.last = val
            self.lastTime = time.ticks_ms()
        elif (val == 0) != self.pressed:
            dif = time.ticks_diff(time.ticks_ms(), self.lastTime)
            if dif > self.debounce:
                # atualiza estado do botao
                self.pressed = val == 0
        return self.pressed
        
# Programa de teste
button = Button(15)
counter = 0
while True:
    print ("Botao apertado {} vezes".format(counter))
    while button.isPressed():
        time.sleep_ms(1)
    while not button.isPressed():
        time.sleep_ms(1)
    counter = counter+1
   
