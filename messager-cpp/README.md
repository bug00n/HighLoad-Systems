# TCP messager

A high-performance asyncronous tcp messager in C++ for Linux featuring:

- Asynchronous connection acceptance
- Using additional thread for reading messages on client
- Writing messages for all connected clients

---

## Requirements

- MinGW with g++ supporting C++17 compatible compiler (e.g., g++ 9+, clang++)
- CMake 3.10 or newer
- Dependencies:
    - [boost.asio](https://github.com/boostorg/asio)

---
# Project structure

messager-cpp/  
├── include/  
|   |   # Header files (.h)   
│   ├── server.h  
│   ├── connection.h  
│   └── user.h  
├── src/   
|   |   # Source files (.cpp)  
│   ├── main.cpp  
│   ├── server.cpp  
│   ├── connection.cpp  
│   └── user.cpp  
├── CMakeLists.txt  
└── README.md  

---

## Building

``` bash
mkdir -p build
cd build
cmake ..
cmake --build .
./server
./client # If you want to use it
```

---


## Testing

Use `test.py` for testing the messager.

``` bash
./server
cd ..
python test/test.py
```

I got that server can handle 1000 clients which send 30 messages in 24 seconds. So 1228 requests per second as total resoult.