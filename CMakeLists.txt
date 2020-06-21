cmake_minimum_required(VERSION 3.16)
project(ecl_rt C)

set(CMAKE_C_STANDARD 99)

add_executable(ecl_rt main.c v3.h image.h)

option(SANITY "Build with sanitizers." OFF)
if (SANITY)
    add_compile_options(-fsanitize=address,undefined)
    link_libraries(-fsanitize=address,undefined)
endif()

find_package(OpenMP)
if(OpenMP_C_FOUND)
    target_link_libraries(ecl_rt PRIVATE OpenMP::OpenMP_C)
endif()
