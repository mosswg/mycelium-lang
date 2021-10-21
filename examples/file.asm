  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/sys.asm"
%include "std/str.asm"
%include "std/file.asm"


global _start

section .data
  filename:      db "examples/file.asm", 0

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

  mov   r10, rsi

  mov   rax, rsi
  call  file~read_lines

  mov   r9, rsi

  mov   rax, r9

; call  arr~printn

  mov   rax, r9
  mov   rbx, str~split
  mov   rcx, type#arr
  mov   rdx, 0x20
  call  arr~for_each

  mov   rax, rsi
  call  arr~printn

  mov   rax, r10
  call  file~close


  ret
