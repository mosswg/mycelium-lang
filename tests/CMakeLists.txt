cmake_minimum_required(VERSION 3.15)
project(mycelium_tests)

include_directories(../src/)

add_executable(run_tests
        ../src/base.cpp
        ../src/parser.cpp
        ../src/tokenizer.cpp
        run_test.cpp
        )