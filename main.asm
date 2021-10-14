  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/out.asm"
%include "std/int.asm"
%include "std/str.asm"
%include "std/arg.asm"
global _start

section .data
  str:      db "Hello"
  number: equ 128
  loop_num: equ 100

section .text

_start:
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
  mov   r8, 1

  .num_loop:
  mov   rax, str
  call  str~println
  add   r8, 1
  cmp   r8, loop_num
  jle   .num_loop

  .return:
  mov   rsi, 0
  ret
