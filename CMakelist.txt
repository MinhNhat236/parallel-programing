cmake_minimum_required(VERSION 3.12)
project("Parallel" CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE ${PROJECT_NAME})
add_definitions(-DOMPI_SKIP_MPICXX) #openmpi
find_package(MPI REQUIRED)

add_executable(${EXECUTABLE} main.cpp)

target_link_libraries(${EXECUTABLE} PUBLIC MPI::MPI_C)

# Настройки компиляторов для лабораторных работ \/
if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(${EXECUTABLE} PRIVATE "-Wall" "-Weffc++" "-Wextra" "-Wpedantic")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  endif()
  target_compile_options(${EXECUTABLE} PRIVATE "/W4" "/utf-8")
endif()