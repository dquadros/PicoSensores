# Exemplo com Teclado

from machine import Pin,Timer
from time import sleep

class Keypad:
    
  # iniciacao
  def __init__(self, rowPins, columnPins, debounce=5):
    self.nr = len(rowPins)
    self.nc = len(columnPins)
    self.debounce = debounce
    self.rows = [Pin(pin, Pin.IN) for pin in rowPins]
    self.columns = [Pin(pin, Pin.IN, Pin.PULL_UP) for pin in columnPins]
    self.curRow = 0
    self.previous = [[[0] for _ in range(self.nc)] for _ in range(self.nr)]
    self.count = [[[self.debounce] for _ in range(self.nc)] for _ in range(self.nr)]
    self.validated = [[[0] for _ in range(self.nc)] for _ in range(self.nr)]
    self.keys = []

  # verifica a proxima linha
  def checkRow(self):
    # coloca a linha em nivel BAIXO
    pin = self.rows[self.curRow]
    pin.init(mode=Pin.OUT)
    pin.low()
    # le as colunas e verifica mudancas
    for col in range(self.nc):
      read = self.columns[col].value()
      if read == self.previous[self.curRow][col]:
        if self.count[self.curRow][col] != 0:
          self.count[self.curRow][col] = self.count[self.curRow][col] - 1
          if self.count[self.curRow][col] == 0:
            # leitura confirmada
            if read != self.validated[self.curRow][col]:
              self.validated[self.curRow][col] = read
              if read == 0:
                  # detectada tecla apertada
                  self.keys.append((self.curRow, col))
      else:
        # reinicia validacao
        self.previous[self.curRow][col] = read
        self.count[self.curRow][col] = self.debounce
    # reconfigura linha como entrada
    pin.init(mode=Pin.IN)
    # passa para a proxima linha
    self.curRow = self.curRow + 1
    if self.curRow == self.nr:
      self.curRow = 0

  # retorna a proxima tecla apertada
  # None se nenhuma
  def getKey(self):
      if len(self.keys) == 0:
        return None
      else:
        return self.keys.pop(0)

pad = Keypad([0, 1, 2, 3], [4, 5, 6])

# tratamento da interrupcao de timer
def scanKeypad(timer):
  pad.checkRow()
    
Timer(mode=Timer.PERIODIC, period=10, callback=scanKeypad)

codes = [['1', '2', '3'], ['4', '5', '6'], ['7', '8', '9'], ['*', '0', '#']]

# laco principal
while True:
  key = pad.getKey()
  if key is None:
    sleep(5)
  else:
    print('{} -> {}'.format(key, codes[key[0]][key[1]]))
  