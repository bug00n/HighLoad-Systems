cmake_minimum_required(VERSION 3.28)
project(HTTP-server)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "src/*.cpp")
add_executable(server ${SOURCES})

add_subdirectory(external/json)
add_subdirectory(external/spdlog)

target_include_directories(server PRIVATE external/json/include external/spdlog/include include)
target_link_libraries(server PRIVATE nlohmann_json spdlog::spdlog ws2_32)
