# Procwatch - Local System Monitor

This project is a Linux system monitoring tool written in C++. It reads system and process information directly from the `/proc` virtual filesystem and can run either in interactive terminal mode or as a background systemd service.

## 1. How to Build
You can build the project using either `Make` or `CMake`.

**Using Make:**
$ make clean
$ make

**Using CMake:**
$ mkdir build && cd build
$ cmake ..
$ make

## 2. How to Run
**Interactive Mode:**
$ ./procwatch interactive
(Press Ctrl+C to exit)

**Background Mode (Requires a config file):**
$ ./procwatch background /path/to/config.txt

## 3. Systemd Service Setup
1. Edit the `procwatch.service` file and update the `ExecStart` path to match your absolute project directory.
2. Copy the service file to systemd directory:
   $ sudo cp procwatch.service /etc/systemd/system/
3. Reload systemd daemon:
   $ sudo systemctl daemon-reload
4. Enable and start the service:
   $ sudo systemctl enable procwatch.service
   $ sudo systemctl start procwatch.service
5. View live logs:
   $ journalctl -u procwatch.service -f
