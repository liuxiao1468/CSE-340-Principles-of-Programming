cmake_minimum_required(VERSION 3.14)
project(proj_2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(proj_2
        inputbuf.cc
        inputbuf.h
        lexer.cc
        lexer.h
        parser.cc
        parser.h
        project2.cc
        project2.h)
