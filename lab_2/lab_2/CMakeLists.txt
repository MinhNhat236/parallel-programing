cmake_minimum_required(VERSION 3.16)

project(lab_2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenMP REQUIRED)

add_executable(lab_2 main.cpp)

target_link_libraries(lab_2 PRIVATE OpenMP::OpenMP_CXX)

if(MSVC)
    target_compile_options(lab_2 PRIVATE /W4)
    target_compile_options(lab_2 PRIVATE $<$<CONFIG:Release>:/O2>)
else()
    target_compile_options(lab_2 PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(lab_2 PRIVATE $<$<CONFIG:Release>:-O3>)
endif()