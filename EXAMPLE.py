## Import
from esp32io import OLED, SDCard
## Init
oled = OLED("/dev/ttyACM0")
sd = SDCard(oled.dev)
## Logic
# SD Commands
sd.list("/")
# Oled Commands
oled.binmap_gif("/gg1.bin","/gg2.bin","/gg3.bin",500,10)
## End
oled.dev.close_serial()

