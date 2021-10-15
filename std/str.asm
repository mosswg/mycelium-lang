  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21
%ifndef _mxs_std_str_
%define _mxs_std_str_

%include "std/sys.asm"
%include "std/out.asm"

; Args:
;   rax: the string whose length is being calculated
; Returns:
;   rsi: the length
str~length:
  push  rbp
  mov   rbp, rsp
  mov   [rbp-12], rax
  push  rbx
  push  rcx
  push  rdx
  mov   rbx, 0

  jmp   .loop_check

  .loop:
    add     rbx, 1
  .loop_check:
    mov     rcx, rax
    add     rcx, rbx
    mov     rdx, [rcx]
    cmp     dl, 0
    jne .loop
  mov   rsi, rbx

  pop   rdx
  pop   rcx
  pop   rbx

  pop   rbp
  ret


; Args
;   rax: the string to be printed
; Returns
;   void
str~print:
  push  rbp
  mov   rbp, rsp
                        ; Perserve the used regiser value
  push  rbx
  push  rsi

  call  str~length
  mov   rbx, rsi
  call  out~puts

  pop   rbx
  pop   rsi
  pop   rbp
  ret


; Args
;   rax: the string to be printed
; Returns
;   void
str~println:
  push  rbp
  mov   rbp, rsp
                        ; Perserve the used regiser value
  push  rdi
  push  rbx
  push  rcx
  push  rdx
  push  r8
  push  rsi

  call  str~length              ; Get the length with the string on rax

  lea   r8, [rax]               ; Get the start of the string
  add   r8, rsi                 ; Move to the end of the string
  mov   dl, 0xA
  mov   bl, [r8+1]                ; Store the current value at the end of the string
  mov   [r8], dl                ; Put the new line at the end of the array
  mov   dl, 0x0
  mov   [r8+1], dl              ; Put a null terminator at the end of the string

  mov   rdx, rsi
  add   rdx, 1                  ; Increase the length of the string since we added a character
  mov   rsi, rax
  mov   rdi, sys~fd~out
  mov   rax, sys~id~write

  syscall                       ; Write the string to sys~out

  mov   dl, 0
  mov   [r8], dl                ; Re-add the null terminator
  mov   [r8+1], bl              ; Restore the old value at r8

  pop   rsi
  pop   r8
  pop   rdx
  pop   rcx
  pop   rbx
  pop   rdi

  pop   rbp
  ret

%endif                          ; ifdef guard
