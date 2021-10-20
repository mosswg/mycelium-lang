cmake_minimum_required(VERSION 3.15)
project(Mycelium_std)

set(CMAKE_ASM_NASM_LINK_EXECUTABLE "ld <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")

include_directories(..)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

add_library(Mycelium_std
        sys.asm
        out.asm
        exception.asm
        mem.asm
        cstr.asm
        int.asm
        args.asm
        arr.asm
        list.asm
        type.asm
        str.asm
        file.asm
        )
