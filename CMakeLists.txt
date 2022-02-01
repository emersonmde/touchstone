cmake_minimum_required(VERSION 3.21)
project(touchstone C)

set(CMAKE_C_STANDARD 23)

add_executable(touchstone main.c repl.c compiler.c compiler.h vm.c vm.h)
