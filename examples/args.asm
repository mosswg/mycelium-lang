  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/sys.asm"
%include "std/arg.asm"
%include "std/str.asm"

global _start

section .data
  arg_name:   db "<arg>", 0

section .text

_start:
  mov   r15, rsp
  call  main
  mov   rax, rsi                ; exit code
  call  sys~exit                ; call exit

main:
  push  arg_name
  mov   rax, 1
  call  arg~require_args
  pop   rbx
  .proper_number_of_args:
  call  arg~number_args
  mov   rbx, 2
  mov   rcx, rsi
  .loop:
    mov   rax, rbx
    call  arg~get
    mov   rax, rsi
    call  str~println
    add   rbx, 1
    .loop_check:
    cmp   rbx, rcx
    jle   .loop
  mov   rsi, 0
  ret
