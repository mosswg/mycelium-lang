  ;; Author: Moss Gallagher
  ;; Date: 20-Oct-21

%include "std/sys.asm"
%include "std/args.asm"
%include "std/str.asm"

global _start

section .data
    arg1:       db "<string> <split start> <split end>", 0

section .text

_start:
    mov   r15, rsp
    call  main
    mov   eax, esi                ; exit code
    call  sys~exit                ; call exit

main:
    mov     rax, 3
    push    arg1
    call    args~require
    pop     rax

    mov     rax, 2
    call    args~get

    mov     r8, rsi

    mov     rax, r8

    call    cstr~println

    mov     rax, 3
    call    args~get

    mov     rax, rsi
    call    str#new_cs

    mov     r9, rsi

    mov     rax, r9
    call    str~to_int

    mov     r10, rsi


    mov     rax, 4
    call    args~get

    mov     rax, rsi
    call    str#new_cs

    mov     r11, rsi

    mov     rax, r11
    call    str~to_int

    mov     r12, rsi


    mov     rax, r8
    call    str#new_cs

    mov     r13, rsi


    mov     rax, r13
    mov     rbx, r10
    mov     rcx, r12

    call    str~substr


    mov     rax, rsi

    call    str~println

    .return:
    mov     rsi, 0
    ret
