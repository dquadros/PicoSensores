# Exemplo do sensor de captura de digitais
from machine import UART, Pin
from time import sleep
from fingersensor_mpython import FingerSensor

# Iniciacao
ledR = Pin(13, Pin.OUT)
ledG = Pin(14, Pin.OUT)
ledB = Pin(15, Pin.OUT)
ledR.off()
ledG.off()
ledB.off()
uart = UART(0, tx=Pin(16), rx=Pin(17))
finger = FingerSensor(uart)
sleep(0.1)
c = finger.count()
if c > 0:
    print ('Apagando {} digitais'.format(c))
    if finger.clear():
        print ('Sucesso')

# Rotina para ler digital e gerar a feature
def captureFeature(numbuf):
    while True:
        ledB.on()
        print ('Coloque o dedo no sensor')
        while not finger.capture():
            sleep(0.05)
        ledB.off()
        print ('Imagem capturada')
        ok = finger.createFeature(numbuf)
        print ('Remova o dedo do sensor')
        sleep(3)
        if ok:
            print('Feature criada')
            return
        ledR.on()
        print ('Imagem ruim, tente de novo')
        sleep(1)
        ledR.off()

# Rotina para cadastrar digital
def enroll():
    first = True
    while True:
        if not first:
            captureFeature(1)
        captureFeature(2)
        if not finger.createModel():
            ledR.on()
            first = False
            print ('Imagens nao combinam, vamos repetir')
            sleep(1)
            ledR.off()
            continue
        pos = finger.count()
        if finger.store(1,pos):
            ledG.on()
            print ('Digital cadastrada na posicao {}'.format(pos))
            sleep(1)
            ledG.off()
            return
        else:
            ledR.on()
            print ('Erro {} ao salvar'.format(finger.lastResponse()))
            sleep(1)
            ledR.off()

# Laco Principal
while True:
    print()
    captureFeature(1)
    print('Procurando...')
    pos = finger.search(1)
    if pos == -1:
        print ('Desconhecida, vamos cadastrar')
        enroll()
    else:
        ledG.on()
        print ('Digital {} reconhecida'.format(pos))
        sleep(1)
        ledG.off()
