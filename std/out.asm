  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21

%ifndef _Mycelium_std_out_
%define _Mycelium_std_out_


%include "std/sys.asm"


; Print the passed string to stdout
; Args
;   rax: the string to be printed
;   rbx: length of the string
; Returns
;   void
out~puts:
  push  rbp
  mov   rbp, rsp

  push  rax             ; Perserve the used regiser values
  push  rbx
  push  rcx
  push  rdx
  push  rsi
  push  rdi

  mov   rdx, rbx
  mov   rsi, rax
  mov   rdi, sys~fd~out
  mov   rax, sys~id~write
  syscall


  pop   rdi
  pop   rsi
  pop   rdx
  pop   rcx
  pop   rbx
  pop   rax

  pop   rbp
  ret


; Print the passed string to stdout
; Args
;   rax: the string to be printed
;   rbx: length of the string
; Returns
;   void
out~put_err:
  push  rbp
  mov   rbp, rsp

  push  rax             ; Perserve the used regiser values
  push  rbx
  push  rcx
  push  rdx
  push  rsi
  push  rdi

  mov   rdx, rbx
  mov   rsi, rax
  mov   rdi, sys~fd~err
  mov   rax, sys~id~write
  syscall


  pop   rdi
  pop   rsi
  pop   rdx
  pop   rcx
  pop   rbx
  pop   rax

  pop   rbp
  ret



; Args
;   rax: the character to be printed
; Returns
;   void
out~putc:
  push  rcx

  push  rdx
  push  rdi
  push  rsi
  push  rbx

  push  rax
  mov   rax, sys~id~write
  mov   rdi, sys~fd~out
  mov   rsi, rsp        ; Use the stack as a pointer to the character we pushed top it
  mov   rdx, 1
  syscall

  pop   rax
  pop   rbx
  pop   rsi
  pop   rdi
  pop   rdx

  pop   rcx
  ret

%endif                          ; ifdef guard
