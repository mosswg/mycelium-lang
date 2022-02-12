cmake_minimum_required(VERSION 3.15)
project(mycelium)

if( BUILD_INTERPRETER )
        add_executable(interpreter
                src/base.cpp
                src/interpreter.cpp
                src/tokenizer.cpp
                )
endif()

if (BUILD_EXAMPLES)
        add_subdirectory(examples)
endif()
