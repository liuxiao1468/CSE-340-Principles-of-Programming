cmake_minimum_required(VERSION 3.14)
project(lexer_parser)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(lexer_parser
        inputbuf.cc
        inputbuf.h
        lexer.cc
        lexer.h
        parser.cc
        parser.h)
