cmake_minimum_required(VERSION 3.23)
project(compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(compiler main.cpp lexer/lexer.cpp lexer/lexeme.cpp)
