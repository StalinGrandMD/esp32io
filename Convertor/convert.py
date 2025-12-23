import argparse
from PIL import Image

WIDTH = 128
HEIGHT = 64
BUF_SIZE = 1024

def main():
    parser = argparse.ArgumentParser(
        description="Convert image to SSD1306 128x64 raw bitmap"
    )
    parser.add_argument("input", help="Input image (png, jpg, etc.)")
    parser.add_argument("output", help="Output binary file (.bin)")
    args = parser.parse_args()

    img = Image.open(args.input).convert("1")
    img = img.resize((WIDTH, HEIGHT))

    pixels = img.load()
    buf = bytearray(BUF_SIZE)

    for page in range(8):
        for x in range(WIDTH):
            byte = 0
            for bit in range(8):
                y = page * 8 + bit
                if pixels[x, y] == 0:  # black pixel = ON
                    byte |= (1 << bit)
            buf[page * WIDTH + x] = byte

    with open(args.output, "wb") as f:
        f.write(buf)

    print(f"OK: {args.output} ({len(buf)} bytes)")

if __name__ == "__main__":
    main()

