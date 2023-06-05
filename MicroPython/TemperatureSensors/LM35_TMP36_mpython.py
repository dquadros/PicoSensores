# Exemplo com os Sensores de Temperatura LM35D e TMP36
from machine import ADC
from time import sleep

# Conexões (-1 se não usado)
pinLM35 = 26
pinTMP36 = 27

if pinLM35 != -1:
    adcLM35 = ADC(pinLM35)
if pinTMP36 != -1:
    adcTMP36 = ADC(pinTMP36)

# retorna a tensão em pino ADC
# faz a média de 10 leituras
def readADC(adc):
    sum = 0
    for i in range(10):
        sum = sum+adc.read_u16()
    return (sum*0.33)/65536

# Laço Principal
while True:
    tempLM35 = 0
    if pinLM35 != -1:
        vLM35 = readADC(adcLM35)
        tempLM35 = vLM35/0.01
    tempTMP36 = 0
    if pinTMP36 != -1:
        vTMP36 = readADC(adcTMP36)
        tempTMP36 = 25.0 + (vTMP36-0.75)/0.01
    print("LM35 = {:.1f}C  TMP36 = {:.1f}C".format(tempLM35, tempTMP36))
    sleep(2)
