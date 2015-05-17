cmake_minimum_required(VERSION 3.0)
add_executable(interpreter file_io.c interpreter.c)
add_executable(test_stack stack.c tests/test_stack.c)
add_executable(test_vector vector.c tests/test_vector.c)
add_definitions(-Wall -Wextra -O3)

if (EMSCRIPTEN)
  set_target_properties(interpreter PROPERTIES LINK_FLAGS
    "--embed-file echo.bf --embed-file hello_world.bf --embed-file hello_world2.bf --embed-file mandelbrot.b --embed-file sierpinski.bf")
else()
  add_executable(compiler file_io.c stack.c compiler.c)
  add_executable(jit file_io.c stack.c vector.c jit.c)
endif()

