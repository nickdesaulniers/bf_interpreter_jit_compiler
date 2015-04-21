cmake_minimum_required(VERSION 3.0)
add_executable(interpreter file_io.c interpreter.c)
add_executable(compiler file_io.c stack.c compiler.c)
add_executable(test_stack stack.c tests/test_stack.c)
add_definitions(-Wall -Wextra -O3)

