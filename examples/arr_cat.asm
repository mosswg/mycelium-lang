    ;; Author: Moss Gallagher
    ;; Data:   28-Oct-21

%include "std/type.asm"
%include "std/arr.asm"


global _start

section .data
  arr_size: equ 10

section .text

_start:
  mov   r15, rsp
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
    mov     rax, arr_size
    mov     rbx, type#int
    call    arr#new

    mov     r9, rsi

    mov     rax, r9
    mov     rbx, 5
    mov     rcx, 1

    call    arr~set


    mov     rax, arr_size
    mov     rbx, type#int
    call    arr#new

    mov     r10, rsi

    mov     rax, r10
    mov     rbx, 5
    mov     rcx, 1

    call    arr~set


    mov     rax, r9
    mov     rbx, r10

    call    arr#concat

    mov     r11, rsi

    mov     rax, rsi
    call    arr~println


    mov     rax, r11
    mov     rbx, r9

    call    arr~concat

    mov     rax, r11
    call    arr~println



    ret
