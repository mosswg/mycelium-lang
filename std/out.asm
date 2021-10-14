  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21

%ifndef _std_out_asm_
%define _std_out_asm_


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



; Args
;   rax: the character to be printed
; Returns
;   void
out~putc:
  push  rbx
  push  rcx
  push  rdx

  push  rax
  mov   rax, sys~id~write
  mov   rbx, sys~fd~out
  mov   rcx, rsp        ; Use the stack as a pointer to the character we pushed top it
  mov   rdx, 1
  syscall

  pop   rax
  pop   rdx
  pop   rsi
  pop   rdi
  ret

%endif
