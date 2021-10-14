  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/sys.asm"
%include "std/str.asm"

global _start

section .data
  str:      db "Hello"
  loop_num: equ 25

section .text

_start:
  mov   r15, rsp
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
