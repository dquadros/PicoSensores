# Exemplo do sensor de captura de digitais
import board
import digitalio
from busio import UART
from time import sleep
from fingersensor_cpython import FingerSensor

# Iniciacao
ledR = digitalio.DigitalInOut(board.GP13)
ledG = digitalio.DigitalInOut(board.GP14)
ledB = digitalio.DigitalInOut(board.GP15)
ledR.direction = digitalio.Direction.OUTPUT
ledG.direction = digitalio.Direction.OUTPUT
ledB.direction = digitalio.Direction.OUTPUT
ledR.value = False
ledG.value = False
ledB.value = False
uart = UART(tx=board.GP16, rx=board.GP17, baudrate=9600*6, bits=8, parity=None, stop=1)
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
        ledB.value = True
        print ('Coloque o dedo no sensor')
        while not finger.capture():
            sleep(0.05)
        ledB.value = False
        print ('Imagem capturada')
        ok = finger.createFeature(numbuf)
        print ('Remova o dedo do sensor')
        sleep(3)
        if ok:
            print('Feature criada')
            return
        ledR.value = True
        print ('Imagem ruim, tente de novo')
        sleep(1)
        ledR.value = False

# Rotina para cadastrar digital
def enroll():
    first = True
    while True:
        if not first:
            captureFeature(1)
        captureFeature(2)
        if not finger.createModel():
            ledR.value = True
            first = False
            print ('Imagens nao combinam, vamos repetir')
            sleep(1)
            ledR.value = False
            continue
        pos = finger.count()
        if finger.store(1,pos):
            ledG.value = True
            print ('Digital cadastrada na posicao {}'.format(pos))
            sleep(1)
            ledG.value = False
            return
        else:
            ledR.value = True
            print ('Erro {} ao salvar'.format(finger.lastResponse()))
            sleep(1)
            ledR.value = False

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
        ledG.value = True
        print ('Digital {} reconhecida'.format(pos))
        sleep(1)
        ledG.value = False
