# Exemplo com DHT11/DHT22

import utime
import rp2 
from rp2 import PIO, asm_pio
from machine import Pin

# Conexoes, -1 = nao usado
pinDHT11 = 16
pinDHT22 = 17

# Programa para a PIO
@asm_pio(set_init=(PIO.OUT_HIGH),autopush=True, push_thresh=8) 
def DHT_PIO():
    # espera o programa disparar
    pull()
     
    # mantem o dado baixo pelo tempo especificado
    set(pindirs,1)              #muda pino para saida
    set(pins,0)                 #coloca o pino no nivel baixo
    mov (x,osr)
    label ('waitx')
    nop() [25] 
    jmp(x_dec,'waitx')          # espera por count*26/clock
      
    # inicia leitura da resposta
    set(pindirs,0)              # muda pino para entrada
    wait(1,pin,0)               # espera voltar ao nivel alto
    wait(0,pin,0)               # espera pulso inicial
    wait(1,pin,0)
    wait(0,pin,0)               # espera inicio do primeiro bit
 
    # le os bits de dados
    label('readdata')
    wait(1,pin,0)               # espera dado alto
    set(x,20)                   # x eh o timeout
    label('countdown')
    jmp(pin,'continue')         # continua contando se dado alto
     
    # pino baixo enquanto contava -> bit 0
    set(y,0)                 
    in_(y, 1)                   # coloca 0 no resultado
    jmp('readdata')             # ler proximo bit
     
    # pino ainda alto
    label('continue')
    jmp(x_dec,'countdown')      # decrementa contagem
 
    # timeout -> bit 1
    set(y,1)                  
    in_(y, 1)                   # coloca 1 no resultado
    wait(0,pin,0)               # espera baixo
    jmp('readdata')             # ler proximo bit
 
DHT11 = 0
DHT22 = 1
 
class DHT:
 
    # Construtor
    # dataPin: pino conectado ao DHT
    # model:   DHT11 ou DHT22
    # smID:    state machine id
    def __init__(self, dataPin, model, smID=0):
        self.dataPin = dataPin
        self.model = model
        self.smID = smID
        self.sm = rp2.StateMachine(self.smID)
        self.lastreading = 0
        self.data=[]
     
    # executa uma leitura
    def read(self):
        data=[]
        self.sm.init(DHT_PIO,freq=1400000,set_base=self.dataPin,in_base=self.dataPin,jmp_pin=self.dataPin)
        self.sm.active(1)
        if self.model == DHT11:
            self.sm.put(969)     # espera 18 ms
        else:
            self.sm.put(54)      # espera 1 ms
        for i in range(5):       # le 5 bytes
            data.append(self.sm.get())
        self.sm.active(0)
        total=0
        for i in range(4):
            total=total+data[i]
        if data[4] == (total & 0xFF):
            # checksum ok, salva os dados
            self.data = data
            self.lastreading = utime.ticks_ms()
            return True
        else:
            return False
 
    # le ou usa dado anterior
    def getData(self):
        # garante que temos dados
        while len(self.data) == 0:
            if not self.read():
                utime.sleep_ms(2000)
             
        # nova leitura so se mais de 2 segundos desde a anterior
        now = utime.ticks_ms()
        if self.lastreading > now:
            self.lastreading = now  # deu a volta
        if (self.lastreading+2000) < now:
            self.read()
     
    # retorna umidade
    def humidity(self):
        self.getData()
        if self.model == DHT11:
            return self.data[0] + self.data[1]*0.1
        else:
            return ((self.data[0] << 8) + self.data[1]) * 0.1
 
    # retorna temperatura
    def temperature(self):
        self.getData()
        if self.model == DHT11:
            return self.data[2] + self.data[3]*0.1
        else:
            s = 1
            if (self.data[2] & 0x80) == 1:
                s = -1
            return s * (((self.data[2] & 0x7F) << 8) + self.data[3]) * 0.1
 
# Programa Principal

if pinDHT11 != -1:
    dht11_data = Pin(pinDHT11, Pin.IN, Pin.PULL_UP)
    dht11 = DHT(dht11_data, DHT11, 0)
    
if pinDHT22 != -1:
    dht22_data = Pin(pinDHT22, Pin.IN, Pin.PULL_UP)
    dht22 = DHT(dht22_data, DHT22, 1)
 
while True:
    if pinDHT11 != -1:
        print("DHT11 Umidade: %.1f%%, Temperatura: %.1fC" % 
          (dht11.humidity(), dht11.temperature()))
    if pinDHT22 != -1:
        print("DHT22 Umidade: %.1f%%, Temperatura: %.1fC" % 
          (dht22.humidity(), dht22.temperature()))
    utime.sleep_ms(3000)
