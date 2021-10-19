  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/sys.asm"
%include "std/args.asm"
%include "std/str.asm"
%include "std/list.asm"
%include "std/int.asm"
%include "std/type.asm"


global _start

section .data
  create_success:     db "Created list with size ", 0
  resize_success:     db "Resized list to size ", 0
  success_post:       db " with an address of ", 0
  first_size:         equ 64
  size_shift:         equ 5
  resize_amount:      equ 4

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
  mov   r10, rsi
  call  int~println

  mov   rcx, 0
  shl   r10, size_shift

  .loop:
    mov   rax, r10
    mov   rbx, rax
    mov   rax, r9
    call  list~resize

    mov   rax, resize_success
    call  str~print

    mov   rax, r9
    call  list~len

    mov   rax, rsi
    call  int~print

    mov   rax, 0xA
    call  out~putc
    shl   r10, size_shift
    add   rcx, 1
  .loop_check:
    cmp rcx, resize_amount
    jl  .loop

  mov   rax, r9
  call  list~del

  mov   rsi, 0
  ret
