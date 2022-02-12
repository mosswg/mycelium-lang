cmake_minimum_required(VERSION 3.15)
project(mycelium)

if( BUILD_INTERPRETER )
        add_executable(interpreter
                src/interpreter.cpp
                )
endif()

if (BUILD_EXAMPLES)
        add_subdirectory(examples)
endif()
