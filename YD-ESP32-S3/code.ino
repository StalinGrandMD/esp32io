#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>
#include <WiFi.h>
#include <WebServer.h>

#define OLED_W 128
#define OLED_H 64

#define SDA_PIN 8
#define SCL_PIN 9

#define SD_CS   10
#define SD_SCK  12
#define SD_MOSI 11
#define SD_MISO 13

Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire);
SPIClass sdSPI(FSPI);

WebServer server(80);
bool wifiOn = false;
File wifiFile;

const char* ssid = "";
const char* password = "";
// Handle HTTP file upload
void handleUpload() {
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        String filename = "/" + upload.filename;

        Serial.print("~UPLOAD_START|");
        Serial.println(filename);

        wifiFile = SD.open(filename, FILE_WRITE);
        if (!wifiFile) {
            Serial.println("~UPLOAD_ERR|OPEN_FAIL");
        }
    }
    else if (upload.status == UPLOAD_FILE_WRITE) {
        if (wifiFile) {
            wifiFile.write(upload.buf, upload.currentSize);
        }
    }
    else if (upload.status == UPLOAD_FILE_END) {
        if (wifiFile) {
            wifiFile.close();
            Serial.print("~UPLOAD_OK|SIZE:");
            Serial.println(upload.totalSize);
        }
    }
    else if (upload.status == UPLOAD_FILE_ABORTED) {
        if (wifiFile) {
            wifiFile.close();
        }
        Serial.println("~UPLOAD_ABORTED");
    }
}

void wifiStart() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("~WIFI_CONNECTING");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.print("~WIFI_ON|IP:");
    Serial.println(WiFi.localIP());

    server.on("/upload", HTTP_POST,
              []() { server.send(200); },
              handleUpload);
    server.begin();
    wifiOn = true;
}

void wifiStop() {
    server.stop();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    wifiOn = false;
    Serial.println("~WIFI_OFF");
}
// ------------------------------------------------------------------


// ---------------------- Existing SD/OLED code ---------------------
void listDirRecursive(fs::FS &fs, const char *dirname, uint8_t levels) {
    File root = fs.open(dirname);
    if (!root) {
        Serial.println("~ERR");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("~ERR");
        root.close();
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("~D:");
            Serial.print(file.name());
            std::string ss = ":" + std::to_string(file.size()) + "\n";
            Serial.print(ss.c_str());
            if (levels) {
                String subPath = file.path();
                file.close();  
                listDirRecursive(fs, subPath.c_str(), levels - 1);
                root = fs.open(dirname);
                if (!root) return;
                file = root.openNextFile();
                continue;
            }
        } else {
            Serial.print("~F:");
            Serial.print(file.name());
            Serial.print(":");
            Serial.print(file.size());
            Serial.println();
        }
        file.close();
        file = root.openNextFile();
    }
    root.close();
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    // I2C OLED
    Wire.begin(SDA_PIN, SCL_PIN);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED_FAIL");
        while (1);
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.display();

    // SD card
    sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, sdSPI)) {
        Serial.println("SD_FAIL");
    } else {
        Serial.println("SD_OK");
    }

    Serial.println("READY");
}
void handleCommand(const String &cmd) {
  
}
void loop() {
    // Wi-Fi HTTP server handler (if active)
    if (wifiOn) server.handleClient();
    String cmd = "NOTAVABIL";
    if (Serial.available()) {
    cmd = Serial.readStringUntil('\n');
    cmd.trim();
    }


    // ---- OLED Commands ----
    if (cmd == "OLED_CLEAR") {
        display.clearDisplay();
        Serial.println("~OK");
    } 
    else if (cmd.startsWith("OLED_TEXT")) {
        int x, y;
        char text[64];
        sscanf(cmd.c_str(), "OLED_TEXT %d %d %[^\n]", &x, &y, text);
        display.setCursor(x, y);
        display.print(text);
        Serial.println("~OK");
    } 
    else if (cmd == "OLED_SHOW") {
        display.display();
        Serial.println("~OK");
    } 
    else if (cmd.startsWith("OLED_PIXEL")) {
        int x, y, v;
        sscanf(cmd.c_str(), "OLED_PIXEL %d %d %d", &x, &y, &v);
        display.drawPixel(x, y, v ? SSD1306_WHITE : SSD1306_BLACK);
        Serial.println("~OK");
    } 
    else if (cmd.startsWith("OLED_LINE")) {
        int x1, y1, x2, y2;
        sscanf(cmd.c_str(), "OLED_LINE %d %d %d %d", &x1, &y1, &x2, &y2);
        display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
        Serial.println("~OK");
    } 
    else if (cmd.startsWith("OLED_RECT")) {
        int x, y, w, h, fill = 0;
        sscanf(cmd.c_str(), "OLED_RECT %d %d %d %d %d", &x, &y, &w, &h, &fill);
        if (fill) display.fillRect(x, y, w, h, SSD1306_WHITE);
        else display.drawRect(x, y, w, h, SSD1306_WHITE);
        Serial.println("~OK");
    } 
    else if (cmd.startsWith("OLED_CIRCLE")) {
        int x, y, r, fill = 0;
        sscanf(cmd.c_str(), "OLED_CIRCLE %d %d %d %d", &x, &y, &r, &fill);
        if (fill) display.fillCircle(x, y, r, SSD1306_WHITE);
        else display.drawCircle(x, y, r, SSD1306_WHITE);
        Serial.println("~OK");
    } 
    else if (cmd.startsWith("OLED_BIN_FULL")) {
        char filename[64];
    
        if (sscanf(cmd.c_str(), "OLED_BIN_FULL %63s", filename) != 1) {
            Serial.println("~ERR");
            return;
        }
    
        File f = SD.open(filename, FILE_READ);
        if (!f || f.size() != 1024) {
            Serial.println("~ERR");
            return;
        }
    
        uint8_t* buffer = display.getBuffer();
        f.read(buffer, 1024);
        f.close();
    
        // 🔁 Invert pixels
        for (int i = 0; i < 1024; i++) {
            buffer[i] = ~buffer[i];
        }
    
        display.display();
        Serial.println("~OK");
    }

    
    else if (cmd.startsWith("OLED_BIN_GIF")) {
        char f1[64], f2[64], f3[64];
        int delayMs, loops;
    
        if (sscanf(cmd.c_str(),
                   "OLED_BIN_GIF %63s %63s %63s %d %d",
                   f1, f2, f3, &delayMs, &loops) != 5) {
            Serial.println("~ERR");
            return;
        }
    
        const char* files[3] = { f1, f2, f3 };
        uint8_t* buffer = display.getBuffer();
    
        for (int l = 0; l < loops; l++) {
            for (int i = 0; i < 3; i++) {
                File f = SD.open(files[i], FILE_READ);
                if (!f || f.size() != 1024) {
                    Serial.println("~ERR");
                    return;
                }
    
                f.read(buffer, 1024);
                f.close();
    
                // Fix inverted colors
                for (int b = 0; b < 1024; b++) {
                    buffer[b] = ~buffer[b];
                }
    
                display.display();
                delay(delayMs);
            }
        }
    
        Serial.println("~OK");
    }



    // ---- SD Commands ----
    else if (cmd.startsWith("SD_WRITE")) {
        char file[32], text[128];
        sscanf(cmd.c_str(), "SD_WRITE %s %[^\n]", file, text);
        File f = SD.open(file, FILE_WRITE);
        if (!f) { Serial.println("~ERR"); return; }
        f.println(text);
        f.close();
        Serial.println("~OK");
    } 
    else if (cmd.startsWith("SD_READ")) {
        char file[32];
        sscanf(cmd.c_str(), "SD_READ %s", file);
        File f = SD.open(file);
        if (!f) { Serial.println("~ERR"); return; }
        Serial.write('~'); 
        while (f.available()) Serial.write(f.read());
        f.close();
        Serial.println();
    } 
    else if (cmd.startsWith("SD_MKDIR")) {
        char path[64];
        sscanf(cmd.c_str(), "SD_MKDIR %s", path);
        if (SD.mkdir(path)) Serial.println("~OK");
        else Serial.println("~ERR");
    } 
    else if (cmd.startsWith("SD_RMDIR")) {
        char path[64];
        sscanf(cmd.c_str(), "SD_RMDIR %s", path);
        if (SD.rmdir(path)) Serial.println("~OK");
        else Serial.println("~ERR");
    } 
    else if (cmd.startsWith("SD_DELETE")) {
        char path[64];
        sscanf(cmd.c_str(), "SD_DELETE %s", path);
        if (SD.remove(path)) Serial.println("~OK");
        else Serial.println("~ERR");
    } 
    else if (cmd.startsWith("SD_RENAME")) {
        char oldp[64], newp[64];
        sscanf(cmd.c_str(), "SD_RENAME %s %s", oldp, newp);
        if (SD.rename(oldp, newp)) Serial.println("~OK");
        else Serial.println("~ERR");
    } 
    else if (cmd.startsWith("SD_LIST_RECURSIVE")) {
        char path[128] = "/";
        int levels = 1;
        sscanf(cmd.c_str(), "SD_LIST_RECURSIVE %s %d", path, &levels);
        int len = strlen(path);
        if (len > 1 && path[len - 1] == '/') path[len - 1] = 0;
        String folder = path;
        if (!folder.startsWith("/")) folder = "/" + folder;
        listDirRecursive(SD, folder.c_str(), levels);
        Serial.println("~END");
    }

    // ---- Wi-Fi LAN Upload Commands ----
    
    else if (cmd == "WIFI") {wifiOn ? wifiStop() : wifiStart();}

    // ---- Unknown Command ----
    else if (cmd != "NOTAVABIL")Serial.println("~UNKNOWN");
    
}
