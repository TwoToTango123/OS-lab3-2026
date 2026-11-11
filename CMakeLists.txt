cmake_minimum_required(VERSION 3.16)
project(os_lab3 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)
add_compile_definitions(_GNU_SOURCE)

find_package(Threads REQUIRED)

add_executable(lab3 src/main.c src/points.c src/search.c)
target_include_directories(lab3 PRIVATE include)
target_link_libraries(lab3 PRIVATE Threads::Threads m)
