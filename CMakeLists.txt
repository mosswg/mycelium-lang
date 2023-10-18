cmake_minimum_required(VERSION 3.15)
project(mycelium)

if( BUILD_INTERPRETER )
	add_compile_options("-Wunused-result")
	add_compile_options("-Wall")
        add_executable(interpreter
                src/base.cpp
                src/interpreter.cpp
                src/tokenizer.cpp
                src/parser.cpp
                src/function.cpp
                )
endif()

if( BUILD_TESTS )
        add_subdirectory(tests)
endif()

if (BUILD_EXAMPLES)
        add_subdirectory(examples)
endif()
