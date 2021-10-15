cmake_minimum_required(VERSION 3.15)
project(examples)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "ld <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_ASM_NASM_FLAGS -g)

enable_language(ASM_NASM)

option(BUILD_STD_LIB "Build the std library into an o file" OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(print
        print.asm
        )

add_executable(args
        args.asm
        )

add_executable(arr
        arr.asm
        )

include_directories(examples ..)
