  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/sys.asm"
%include "std/arg.asm"
%include "std/str.asm"

global _start

section .data
  err: db "Usage: ./arg <arg>", 0

section .text

_start:
  mov   r15, rsp
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
  call  arg~argc
  cmp   rsi, 1
  jne   .proper_number_of_args
  mov   rax, err
  call  str~println
  mov   rax, 1
  call  sys~exit
  .proper_number_of_args:
  mov   rbx, 1
  mov   rcx, rsi
  .loop:
    add   rbx, 1
    mov   rax, rbx
    call  arg~get
    mov   rax, rsi
    call  str~println
    .loop_check:
    cmp   rbx, rcx
    jl   .loop
  mov   rsi, 0
  ret
