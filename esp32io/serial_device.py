# serial_device_py.py

class SerialDevice:
    def __init__(self, port, baudrate=115200):
        # C++ pybind module
        import serial_device
        self.dev = serial_device.SerialDevice(port, baudrate)

    # ---- TEXT COMMANDS ----
    def write_line(self, command: str):
        self.dev.write_line(command)

    def read_line(self) -> str:
        return self.dev.read_line()

    

    def close_serial(self):
        self.dev.close_serial()

    # ---- CONTEXT MANAGER ----
    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close_serial()

