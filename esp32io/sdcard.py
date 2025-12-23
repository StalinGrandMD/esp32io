class SDCard:
    def __init__(self, serial_device):
        self.dev = serial_device

    def write(self, filename, text):
        self.dev.write_line(f"SD_WRITE {filename} {text}")
    
    def send_file(self, local_path: str):
        import time
        import requests
        import os


        # 1️⃣ Turn Wi-Fi on
        self.dev.write_line("WIFI")
        time.sleep(5)

        # 2️⃣ Get ESP32 IP
        esp_ip = ""
        for i in range(10):
            line = self.dev.read_line()
            if line.startswith("WIFI_ON|IP:"):
                esp_ip = line.split(":")[-1]
                break
        if not esp_ip:
            raise RuntimeError("ESP32 did not report IP")

        # 3️⃣ Upload file (fresh open every time)
        with open(local_path, "rb") as f:
            files = {"file": (os.path.basename(local_path), f)}
            r = requests.post(f"http://{esp_ip}/upload", files=files)
            print(r.status_code, r.text)
        print("Uploaded", local_path)

        # 4️⃣ Turn Wi-Fi off
        self.dev.write_line("WIFI")
        time.sleep(0.5)

    def read(self, filename):
        self.dev.write_line(f"SD_READ {filename}")
        return self.dev.read_line()

    def mkdir(self, path):
        self.dev.write_line(f"SD_MKDIR {path}")      

    def rmdir(self, path):
        self.dev.write_line(f"SD_RMDIR {path}")

    def delete(self, path):
        self.dev.write_line(f"SD_DELETE {path}")

    def rename(self, old, new):
        self.dev.write_line(f"SD_RENAME {old} {new}")

    def list(self, path="/", levels=0, timeout=5):
        import time
        
        self.dev.write_line(f"SD_LIST_RECURSIVE {path} {levels}")

        files = []
        start = time.time()

        while True:
            if time.time() - start > timeout:
                print("Warning: SD_LIST_RECURSIVE timeout")
                break

            line = self.dev.read_line()
            if not line:
                continue

            if line == "END":
                break

            if ":" not in line:
                continue

            typ, name, size = line.split(":", 2)
            try:
                files.append((typ, name, int(size)))
            except ValueError:
                continue
        for f in files:
            print(f)

