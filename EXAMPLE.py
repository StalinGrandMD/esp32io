## Import
from esp32io import OLED, SDCard
## Init
oled = OLED("/dev/ttyACM0")
sd = SDCard(oled.dev)
## Logic
# SD Commands
sd.list("/")
# Oled Commands
oled.clear()
oled.text("Is this real?", 0, 0)
oled.circle(64, 32, 10, True)
oled.rect(39, 20, 50, 24)
oled.show()
## End
oled.dev.close_serial()

