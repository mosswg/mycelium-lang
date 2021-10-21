  ;; Author: Moss Gallagher
  ;; Date: 20-Oct-21

%include "std/sys.asm"
%include "std/args.asm"
%include "std/str.asm"

global _start

section .data
    arg1: db "<number>", 0

section .text

_start:
    mov   r15, rsp
    call  main
    mov   eax, esi                ; exit code
    call  sys~exit                ; call exit

main:
    mov     rax, 1
    push    arg1
    call    args~require
    pop     rax

    mov     rax, 2
    call    args~get

    mov     rax, rsi
    call    str#new_cs

    mov     r9, rax
    call    str~to_int

    mov     r10, rsi

    mov     rax, rsi
    call    int~println


    mov     rax, r10
    shl     rax, 1

    call    int~println

    ret
