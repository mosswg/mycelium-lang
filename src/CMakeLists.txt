cmake_minimum_required(VERSION 3.15)
project(mycelium_interpreter)



if ( BUILD_INTERPRETER )
add_executable(mycelium_interpreter
            base.cpp
            interpreter.cpp
        )
endif()