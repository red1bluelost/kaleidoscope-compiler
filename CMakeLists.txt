cmake_minimum_required(VERSION 3.22)
project(kaleidoscope)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        $<$<CONFIG:DEBUG>:-fsanitize=address>
        $<$<CONFIG:DEBUG>:-g3>
        $<$<CONFIG:DEBUG>:-Og>
        $<$<CONFIG:RELEASE>:-g0>
        $<$<CONFIG:RELEASE>:-O3>)

add_link_options(
        $<$<CONFIG:DEBUG>:-fsanitize=address>)

include_directories(include)

add_executable(kaleidoscope main.cpp lib/Lexer/Lexer.cpp include/Lexer/Lexer.h)
