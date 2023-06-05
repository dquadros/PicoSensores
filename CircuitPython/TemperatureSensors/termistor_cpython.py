# Exemplo com Termistor
import math
import analogio
import board
from time import sleep

# Parâmetros do Circuito
Vcc = 3.3
R = 2200.0

# Termistor está no ADC0
sensor = analogio.AnalogIn(board.A0)

# Retorna a resistência do termistor
# usa a média de 20 leituras do ADC
def val():
    soma = 0
    for i in range(20):
        soma = soma + sensor.value;
    v = (Vcc*soma)/(20*65536.0)
    return R*(Vcc-v)/v

# Obtem uma referência (temperatura + resistencia)
def getRef():
    while True:
        x = input('Temperatura: ')
        try:
            t = float(x)
            r = val()
            print('Resistencia: {:.0f}'.format(r))
            print()
            return 273.0 + t, r
        except ValueError:
            print ('Formato invalido!')

# Calcula o beta a partir de duas referências
print('Referencia 1:')
t1, rt1 = getRef()
print('Referencia 2:')
t2, rt2 = getRef()
beta = math.log(rt1/rt2)/((1/t1)-(1/t2))
print('Beta = {:.2f}'.format(beta))
rx = rt1 * math.exp(-beta/t1)

# Laco Principal
while True:
    rt = val()
    t = beta / math.log(rt/rx)
    print ('Temperatura: {:.1f}'.format(t-273.0))
    sleep(1)
    

