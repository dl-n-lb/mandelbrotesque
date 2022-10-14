cmake_minimum_required(VERSION 3.22)

project(mandelbrotesque)

set(CMAKE_CXX_COMPILER "/usr/bin/g++") # remove this or change if your compiler is different
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(common_cxx/)

add_executable(mandelbrotesque source/main.cxx)
target_link_libraries(mandelbrotesque PRIVATE raylib)
