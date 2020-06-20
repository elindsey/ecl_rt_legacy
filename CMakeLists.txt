cmake_minimum_required(VERSION 3.16)
project(ecl_rt C)

set(CMAKE_C_STANDARD 99)

add_executable(ecl_rt main.c v3.h bmp.h)