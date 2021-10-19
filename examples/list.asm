  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/sys.asm"
%include "std/arg.asm"
%include "std/str.asm"
%include "std/list.asm"
%include "std/int.asm"
%include "std/type.asm"


global _start

section .data
  create_success:     db "Created list with size ", 0
  write_success:      db "Wrote Values to list: ", 0
  push_success_pre:   db "Push value ", 0
  push_success_post:  db " to array: ", 0
  resize_success:     db "Resized list to size ", 0
  success_post:       db " with an address of ", 0
  first_size:         equ 8
  new_size:           equ 16
  push_value:         db "Test str", 0


section .text

_start:
  mov   r15, rsp
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
  mov   rax, list#type#basic
  mov   rbx, first_size

  call  list#new
  mov   r9, rsi

  mov   rax, create_success
  call  str~print

  mov   rax, r9
  call  list~len

  mov   rax, rsi
  call  int~print

  mov   rax, success_post
  call  str~print

  mov   rax, r9
  call  int~println

  mov   rax, r9
  call  list~println

  mov   rax, 0xA
  call  out~putc


  mov   rbx, 0
  .loop:
      mov   rax, r9
      mov   rcx, rbx
      mov   rdx, type#int
      call  list~set
      add   rbx, 1
  .loop_check:
      cmp   rbx, first_size
      jl    .loop


  mov   rax, write_success
  call  str~print

  mov   rax, r9
  call  list~println

  mov   rax, 0xA
  call  out~putc


  mov   rax, r9
  mov   rbx, push_value
  mov   rcx, type#string
  call  list~push
  mov   r9, rax

  mov   rax, push_success_pre
  call  str~print

  mov   rax, '"'
  call  out~putc

  mov   rax, push_value
  call  str~print

  mov   rax, '"'
  call out~putc

  mov   rax, push_success_post
  call  str~print

  mov   rax, r9
  call  list~println

  mov   rax, 0xA
  call  out~putc

  mov   rax, r9
  mov   rbx, new_size
  call  list~resize
  mov   r9, rax

  mov   rax, resize_success
  call  str~print

  mov   rax, r9
  call  list~len

  mov   rax, rsi
  call  int~print

  mov   rax, success_post
  call  str~print

  mov   rax, r9
  call  int~println

  mov   rax, r9
  call  list~println

  mov   rax, r9
  call  list~del

  mov   rsi, 0
  ret
