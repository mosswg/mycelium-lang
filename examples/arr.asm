    ;; Author: Moss Gallagher
    ;; Data:   19-Oct-21

%include "std/type.asm"
%include "std/arr.asm"


global _start

section .data
  arr_size: equ 25

section .text

_start:
  mov   r15, rsp
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
    mov     rax, type#int
    mov     rbx, arr_size
    call    arr#new

    mov     r9, rsi

    mov     rax, r9
    call    arr~printn

    ret
