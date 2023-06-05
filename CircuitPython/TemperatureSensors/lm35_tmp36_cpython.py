# Exemplo com os Sensores de Temperatura LM35D e TMP36
import analogio
import board
from time import sleep

# Conexões (-1 se não usado)
pinLM35 = board.A0
pinTMP36 = board.A1

if pinLM35 != -1:
    adcLM35 = analogio.AnalogIn(pinLM35)
if pinTMP36 != -1:
    adcTMP36 = analogio.AnalogIn(pinTMP36)

# retorna a tensão em pino ADC
# faz a média de 10 leituras
def readADC(adc):
    sum = 0
    for i in range(10):
        sum = sum+adc.value
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

