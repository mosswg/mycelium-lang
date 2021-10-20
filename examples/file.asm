  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/sys.asm"
%include "std/str.asm"
%include "std/file.asm"


global _start

section .data
  filename:      db "README.md", 0

section .text

_start:
  mov   r15, rsp
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
  mov   rax, filename
  mov   rbx, file#flags#create | file#mode#read_write
  mov   rcx, file#perm#rw
  call  file~open

  mov   rax, rsi
  call  file~read_lines


  ret
