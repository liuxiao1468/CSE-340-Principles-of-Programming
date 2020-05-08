cmake_minimum_required(VERSION 3.14)
project(CSE340F19_Project4_Provided_Code)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(CSE340F19_Project4_Provided_Code
        compiler.cc
        compiler.h
        demo.cc
        inputbuf.cc
        inputbuf.h
        ir_debug.cc
        ir_debug.h
        lexer.cc
        lexer.h
        parser.cc
        parser.h)
