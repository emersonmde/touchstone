cmake_minimum_required(VERSION 3.21)
project(touchstone C)

set(CMAKE_C_STANDARD 23)

add_executable(touchstone src/main.c src/repl.c src/repl.h src/compiler.c src/compiler.h src/db.c src/db.h)
