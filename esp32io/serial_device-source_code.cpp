#include <pybind11/stl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string>

namespace py = pybind11;

class SerialDevice {
private:
    int serial;
    std::string buffer;

public:
    SerialDevice(const std::string& port, int baudrate = 115200) {
        serial = open(port.c_str(), O_RDWR | O_NOCTTY);
        if (serial < 0) throw std::runtime_error("Error opening serial port");

        termios tty{};
        if (tcgetattr(serial, &tty) != 0) { 
            close(serial); 
            throw std::runtime_error("Error getting attributes"); 
        }

        cfsetispeed(&tty, baudrate);
        cfsetospeed(&tty, baudrate);
        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        tty.c_cflag |= CLOCAL | CREAD;
        tty.c_cflag &= ~(PARENB | PARODD);
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;
        tty.c_lflag = 0;
        tty.c_oflag = 0;
        tty.c_iflag = 0;
        tcsetattr(serial, TCSANOW, &tty);
    }

    ~SerialDevice() {
        close_serial();
    }

    void write_line(const std::string& line)
    {
        if (serial < 0) return;

        std::string data = line + "\n";
        const char* buf = data.c_str();
        size_t total = 0;
        size_t len = data.size();

        while (total < len) {
            ssize_t n = write(serial, buf + total, len - total);
            if (n < 0) {
                if (errno == EINTR) continue;
                perror("Serial write failed");
                return;
            }
            total += n;
        }
    }

    std::string read_line() {
        if (serial < 0) return "ERR";

        char ch;
        while (read(serial, &ch, 1) > 0) {
            if (ch == '\n') {
                std::string line = buffer;
                buffer.clear();
                if (!line.empty() && line[0] == '~') {
                    line.erase(0,1);
                    return line;
                }
            } else if (ch != '\r') {
                buffer += ch;
            }
        }
        return "ERR";
    }

    void close_serial() {
        if (serial >= 0) { 
            close(serial); 
            serial = -1; 
        }
    }

    // Python context manager support
    SerialDevice* enter() { return this; } // __enter__
    void exit(py::object exc_type, py::object exc_value, py::object traceback) { // __exit__
        close_serial();
    }
};

// pybind11 binding
PYBIND11_MODULE(serial_device, m) {
    py::class_<SerialDevice>(m, "SerialDevice")
        .def(py::init<const std::string&, int>(), py::arg("port"), py::arg("baudrate")=115200)
        .def("write_line", &SerialDevice::write_line)
        .def("read_line", &SerialDevice::read_line)
        .def("close_serial", &SerialDevice::close_serial)
        .def("__enter__", &SerialDevice::enter)
        .def("__exit__", &SerialDevice::exit, py::arg("exc_type"), py::arg("exc_value"), py::arg("traceback"));
}
