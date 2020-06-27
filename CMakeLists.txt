cmake_minimum_required(VERSION 3.16)
project(ecl_rt C)

set(CMAKE_C_STANDARD 99)

# unsupported on macos ld
#set(CMAKE_LINK_WHAT_YOU_USE TRUE)

option(SANITY "Build with sanitizers." OFF)
if (SANITY)
    message(STATUS "Enabling sanity")
    add_compile_options(-fsanitize=address,undefined)
    link_libraries(-fsanitize=address,undefined)
endif()

add_executable(ecl_rt src/main.c src/v3.h src/image.h src/ecl.h)

find_program(IWYU_PATH NAMES include-what-you-use iwyu)
if(IWYU_PATH)
    message(STATUS "Enabling iwyu")
    set_property(TARGET ecl_rt PROPERTY C_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})
endif()

#find_package(OpenMP)
#if(OpenMP_C_FOUND)
#    target_link_libraries(ecl_rt PRIVATE OpenMP::OpenMP_C)
#endif()
