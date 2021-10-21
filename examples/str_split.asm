  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/sys.asm"
%include "std/str.asm"

global _start

section .data
    str:      db "Hello World Foo Bar", 0

section .text

_start:
    mov   r15, rsp
    call  main
    mov   eax, esi                ; exit code
    call  sys~exit                ; call exit

main:
    mov     rax, str
    call    str#new_cs

    mov     r9, rax
    call    str~println


    mov     rax, r9
    mov     rbx, 0x20
    call    str~split

    mov     rax, rsi
    call    arr~println

    ret
