cmake_minimum_required(VERSION 3.14)
project(CSE340F19_Provided_Code)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(CSE340F19_Provided_Code
        inputbuf.cc
        inputbuf.h
        lexer.cc
        lexer.h
        parser.cc
        parser.h)
