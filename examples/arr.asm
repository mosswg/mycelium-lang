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
  err:      db "Usage: ./arg <size>", 0
  success:  db "Created array with size: ", 0
  resize_success:  db "Resized array to size: ", 0
  first_size:     equ 128
  new_size:       equ 256

section .text

_start:
  mov   r15, rsp
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
  call  arg~argc
  cmp   rsi, 1
  jne   .valid_arg

  mov   rax, err
  call  str~println
  mov   rax, 1
  call  sys~exit

  .valid_arg:
    mov   rax, first_size
    mov   rbx, type~int

    call  arr~new
    mov   r9, rsi

    call  arr~len

    mov   rax, success
    call  str~print
    mov   rax, rsi
    call  int~println

    mov   rax, r9
    mov   rbx, new_size
    call  arr~resize

    mov   rax, rsi
    call  arr~len


    mov   rax, resize_success
    call  str~print
    mov   rax, rsi
    call  int~println

  
    mov   rax, r9
    call  arr~del

  mov   rsi, 0
  ret
