cmake_minimum_required(VERSION 3.15)
project(Mycelium)

add_subdirectory(src)

if (BUILD_EXAMPLES)
        add_subdirectory(examples)
endif()
