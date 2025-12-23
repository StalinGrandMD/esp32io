from .serial_device import SerialDevice

class OLED:
    def __init__(self, port):
        self.dev = SerialDevice(port)

    def clear(self):
        self.dev.write_line("OLED_CLEAR")

    def text(self, text, x, y):
        self.dev.write_line(f"OLED_TEXT {x} {y} {text}")

    def pixel(self, x, y, value=1):
        self.dev.write_line(f"OLED_PIXEL {x} {y} {value}")

    def show(self):
        self.dev.write_line(f"OLED_SHOW")

    def line(self, x1, y1, x2, y2):
        self.dev.write_line(f"OLED_LINE {x1} {y1} {x2} {y2}")

    def rect(self, x, y, w, h, fill=False):
        self.dev.write_line(f"OLED_RECT {x} {y} {w} {h} {int(fill)}")

    def circle(self, x, y, r, fill=False):
        self.dev.write_line(f"OLED_CIRCLE {x} {y} {r} {int(fill)}")

    def binmap(self,filename):
        self.dev.write_line(f"OLED_BIN_FULL {filename}")

    def binmap_gif(self, filenameA,filenameB,filenameC,timeintervalms=120,timesloop=2):
        self.dev.write_line(f"OLED_BIN_GIF {filenameA} {filenameB} {filenameC} {timeintervalms} {timesloop}")

