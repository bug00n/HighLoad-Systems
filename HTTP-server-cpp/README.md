# HighLoad HTTP Server

A high-performance HTTP 1.1 server in C++ for Windows (MinGW) featuring:

- Manual HTTP request parsing (GET, POST)
- Multithreading with thread pool
- Response caching
- Thread-safe synchronous logging using spdlog
- JSON processing with nlohmann/json

---

## Features

- Uses Windows Sockets API (Winsock)
- File and console logging with custom format
- Thread-safe cache with TTL (time-to-live)
- Parses HTTP 1.1 requests and generates proper responses
- Supports simple JSON POST requests
- Thread pool for handling concurrent clients

---

## Requirements

- Windows 7 or later
- MinGW with g++ supporting C++20
- CMake 3.20 or newer
- Dependencies:
    - [spdlog](https://github.com/gabime/spdlog)
    - [nlohmann/json](https://github.com/nlohmann/json)

---

## Building

Move all the files into the HTTP-server folder.

Download spdlog and nlohmann/json. Add them into `\external`.

Then if you're using cmd, this code might help you:
```bash
cd C:\Users\User\HTTP-server\
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
server.exe
```

---

## Example Requests

- ```curl http://localhost:8080/path```
- ```curl -X POST http://localhost:8080/data -H "Content-Type: application/json" -d "{\"name\":\"Alice\", \"age\":25}"```

---

## Testing

- ```ab -n 1000 -c 100 -v 2 http://localhost:8080/```
