cmake_minimum_required(VERSION 3.10)
project(messager-cpp CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(server src/main.cpp)
add_executable(client client/client.cpp)

target_include_directories(server PRIVATE include)
target_link_libraries(server PRIVATE boost_system pthread)