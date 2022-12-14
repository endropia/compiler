cmake_minimum_required(VERSION 3.23)
project(compiler)

set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME magic_enum
        GITHUB_REPOSITORY Neargye/magic_enum
        GIT_TAG v0.8.1
)

add_executable(compiler main.cpp lexer/lexer.cpp lexer/lexeme.cpp parser/parser.cpp parser/parser.h args.cpp args.h)
add_executable(compiler_tests tests/test.cpp lexer/lexer.cpp lexer/lexeme.cpp parser/parser.cpp parser/parser.h tests/tester.cpp tests/tester.h args.cpp args.h)

target_link_libraries(compiler magic_enum::magic_enum)
target_link_libraries(compiler_tests magic_enum::magic_enum)
