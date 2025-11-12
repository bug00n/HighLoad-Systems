cmake_minimum_required(VERSION 3.28)
project(ImageProcessor)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "src/*.cpp")
add_executable(image_processor image_processor.cpp ${SOURCES})
add_executable(tests_image_processor test.cpp ${SOURCES})

target_include_directories(image_processor PRIVATE include)
target_include_directories(tests_image_processor PRIVATE include)
