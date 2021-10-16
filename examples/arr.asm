  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/sys.asm"
%include "std/arg.asm"
%include "std/str.asm"
%include "std/arr.asm"
%include "std/int.asm"
%include "std/type.asm"


global _start

section .data
  err:              db "Usage: ./arg <size>", 0
  create_success:   db "Created array with size ", 0
  write_success:    db "Wrote Values to array: ", 0
  resize_success:   db "Resized array to size ", 0
  success_post:     db " with an address of ", 0
  first_size:       equ 8
  new_size:         equ 16

section .text

_start:
  mov   r15, rsp
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
  mov   rax, first_size
  mov   rbx, type~int

  call  arr~new
  mov   r9, rsi

  mov   rax, create_success
  call  str~print

  mov   rax, r9
  call  arr~len

  mov   rax, rsi
  call  int~print

  mov   rax, success_post
  call  str~print

  mov   rax, r9
  call  int~println

  mov   rax, r9
  call  arr~println

  mov   rax, 0xA
  call  out~putc



  mov   rbx, 0
  .loop:
      mov   rax, r9
      mov   rcx, rbx
      call  arr~set
      add   rbx, 1
  .loop_check:
      cmp   rbx, first_size
      jl    .loop

  mov   rax, write_success
  call  str~print

  mov   rax, r9
  call  arr~println

  mov   rax, 0xA
  call  out~putc

  mov   rax, r9
  mov   rbx, new_size
  call  arr~resize
  mov   r9, rsi

  mov   rax, resize_success
  call  str~print

  mov   rax, r9
  call  arr~len

  mov   rax, rsi
  call  int~print

  mov   rax, success_post
  call  str~print

  mov   rax, r9
  call  int~println

  mov   rax, r9
  call  arr~println

  mov   rax, r9
  call  arr~del

  mov   rsi, 0
  ret
