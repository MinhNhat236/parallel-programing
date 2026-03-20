cmake_minimum_required(VERSION 3.16)

project(lab3 LANGUAGES CXX)

find_package(MPI REQUIRED)

add_executable(lab3 main.cpp)

target_compile_features(lab3 PRIVATE cxx_std_20)
target_link_libraries(lab3 PRIVATE MPI::MPI_CXX)