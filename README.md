# esp32io

A Python library for controlling an **ESP32** with an **SSD1306 OLED (128×64)** display and an **SD card**, using **USB serial communication**.

---

## ✨ Features

- Control SSD1306 OLED display (128×64, monochrome)
- Manage files and folders on an SD card
- Send files over Wi-Fi
- Simple Python API
- Supports bitmap images and GIF-like animations on OLED

---

## ⚠️ Important Notes
- The Code **It has AI code**
- **If you find a bug dont report it just make a fork or just modify it, why? because im lazy and dont care**
- **SD commands must be executed before OLED commands**
- All filenames and paths **must start with `/`**
  - `/file.txt`
  - `/folder/file.txt`
- The **ESP32 and PC must be on the same network** for Wi-Fi file transfer
- Wi-Fi **automatically stops after sending a file**
- The **Program its working only linux mint, if have other you need to compile serial_device-source_code.cpp in esp32io folder** the compilation command is `c++ -O3 -Wall -shared -std=c++17 -fPIC     $(python3 -m pybind11 --includes)     serial_device-source_code.cpp -o serial_device$(python3-config --extension-suffix)`

---

## 📁 SD Card Commands

sd.write(filename, text)             # Write text to a file

sd.send_file(pathfile)               # Send a file using Wi-Fi

sd.read(filename)                    # Read text from a file

sd.mkdir(path)                       # Create a directory

sd.rmdir(path)                       # Remove a directory

sd.delete(filename)                  # Delete a file

sd.rename(oldfilename, newfilename)  # Rename a file

sd.list(path)                        # List directory contents

## 🖥️ OLED Commands

oled.clear()                         # Clear display (black screen)

oled.text(text, x, y)                # Draw text

oled.pixel(x, y, value=1)            # Draw a pixel (1 = white, 0 = black)

oled.show()                          # Update display

oled.line(x1, y1, x2, y2)            # Draw a line

oled.rect(x, y, w, h, fill)          # Draw a rectangle

oled.circle(x, y, r, fill)           # Draw a circle

oled.binmap(filename.bin)

- Display a binary monochrome 128x64 image
- oled.show() is called internally

oled.binmap_gif(fileA.bin, fileB.bin, fileC.bin, msdelay, loops)

- Emulate a GIF animation using multiple frames
- oled.show() is called internally

---

## 🖼️ PNG to BIN Converter
- PNG images must be exactly 128×64
- Output must be monochrome binary
- Used for oled.binmap() and oled.binmap_gif()

---

## 🚀 Example 
- See the EXAMPLE.py file for mini example

---

## 📜 License
- This project is licensed under the GNU General Public License v3.0.
- You are free to:
- Use
- Modify
- Distribute
- Share improvements
- Under the condition that source code remains open and the same license is preserved.
- See the LICENSE file for full details.
