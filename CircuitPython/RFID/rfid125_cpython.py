# Exemplo do leitor RFID 125kHz
import board
import digitalio
import pwmio
from busio import UART
from time import sleep, monotonic_ns

def time_ms():
    return monotonic_ns() // 1000000

# Classe para tratar o buzzer
class Buzzer:
    def __init__(self, pin):
        self.pinBuzzer = digitalio.DigitalInOut(pin)
        self.pinBuzzer.direction = digitalio.Direction.OUTPUT
        self.pinBuzzer.value = False
        
    def beep(self):
        self.pinBuzzer.value = True
        sleep(.3)
        self.pinBuzzer.value = False

# Classe para tratar o botao
class Button:
    def __init__(self, pin, debounce=20):
        self.pinButton = digitalio.DigitalInOut(pin)
        self.pinButton.pull = digitalio.Pull.UP
        self.pressed = False
        self.debounce = debounce
        self.last = self.pinButton.value
        self.lastTime = time_ms()
        
    def released(self):
        val = self.pinButton.value
        if val != self.last:
            # leitura mudou
            self.last = val
            self.lastTime = time_ms()
        elif (val == 0) != self.pressed:
            dif = time_ms() - self.lastTime
            if dif > self.debounce:
                # atualiza estado do botao
                self.pressed = val == 0
                return not self.pressed
        return False
        
# Classe para tratar o servo
class Servo:
    def __init__(self, pin, tempo0=0.6, tempo180=2.4):
        self.pwmServo = pwmio.PWMOut(pin, duty_cycle=0, frequency=50)
        self.tempo0 = tempo0
        self.tempo180 = tempo180
        
    def pos(self, angulo):
        tempo = self.tempo0+(self.tempo180-self.tempo0)*angulo/180
        val = tempo * 65535/20
        self.pwmServo.duty_cycle = int(val)
        sleep(0.3)
        self.pwmServo.duty_cycle = 0

# Classe para ler tags
class RFID:
    def __init__(self, pin):
        self.uart = UART(tx=None, rx=pin, baudrate=9600, bits=8, parity=None, stop=1)
        self.last = ''
        self.last_read = 0
        self.bufRx = bytearray(14)
        self.pos = 0
        
    def read(self):
        if self.uart.in_waiting == 0:
            if (self.last != '') and ((time_ms()- self.last_read) > 1000):
                # Muito tempo sem ler, esquece tag anterior
                self.last = ''
            return None
        rx = self.uart.read(1)
        if rx == None:
            return None
        c = rx[0]
        if self.pos==0 and c!=0x02:
            return None
        self.bufRx[self.pos] = c
        self.pos = self.pos+1
        if self.pos == 14:
            self.pos = 0
            if c == 0x03:
                # recebeu mensagem completa
                self.last_read = time_ms()
                crc = 0
                for i in range(1,13,2):
                    x = self.bufRx[i:i+2].decode()
                    crc = crc ^ int(x,16)
                if crc == 0:
                    tag = self.bufRx[3:11].decode()
                    if tag != self.last:
                        self.last = tag
                        return tag
        return None

# Cria os nossos objetos
led = digitalio.DigitalInOut(25)
led.direction = digitalio.Direction.OUTPUT
buzzer = Buzzer(board.GP2)
button = Button(board.GP16)
servo = Servo(board.GP17)
rfid = RFID(board.GP13)

# Pisca o LED, "fecha a porta" e dá um beep
led.value = True
buzzer.beep()
servo.pos(0)
led.value = False

# Lista principal
autorized = set()
cadastro = False
fechar = None
while True:
    if (fechar != None) and (time_ms() > fechar):
        servo.pos(0)
        fechar = None
    if button.released():
        # apertou e soltou o botão
        cadastro = True
    tag = rfid.read()
    if tag != None:
        if cadastro:
            if tag in autorized:
                print ('Tag {} ja estava cadastrada'.format(tag))
            else:
                led.value = True
                autorized.add(tag)
                print ('Tag {} cadastrada'.format(tag))
                sleep(0.3)
                led.value = False
            cadastro = False
        else:
            if tag in autorized:
                print ('Tag {} autorizada'.format(tag))
                servo.pos(180) # abre a porta
                fechar = time_ms()+3000
            else:
                print ('Tag {} NAO autorizada'.format(tag))
                buzzer.beep()
