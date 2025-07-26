# Echo Server with Logging

## Overview
This project implements a simple TCP echo server written in **C++20** using the **Winsock2 API** on Windows.  
It accepts client connections, echoes back received messages, and logs all events (connections, messages, errors) both to a file and the console.

---

## Features

- TCP echo server running on port **8080**
- Thread-safe logging to `server.log` and console output
- Proper Winsock initialization and cleanup
- Synchronous handling of client connections

---

## Requirements

- Windows 7 or later (_using `_WIN32_WINNT 0x0601`_)
- C++20 compatible compiler (I used MinGW-w64)
- CMake for build automation (or link winsock manually)

---

## Build Instructions

### Using CLion

1. Open `server.cpp` in CLion.
2. Add `CMakeLists.txt` to the project.
3. CLion will automatically detect the CMake file and configure the project.
4. Click the Build button or run the build target to compile the executable.
5. Run the echo_server executable inside CLion or from the terminal.

### Using MinGW-w64 (Command Line)
g++ -std=c++20 server.cpp -o server.exe -lws2_32
