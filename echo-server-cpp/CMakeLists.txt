cmake_minimum_required(VERSION 3.28)
project(echo_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(server server.cpp)
target_link_libraries(server ws2_32)
