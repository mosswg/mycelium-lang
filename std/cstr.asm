  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21
%ifndef _Mycelium_std_cstr_
%define _Mycelium_std_cstr_

%include "std/sys.asm"
%include "std/out.asm"


; Args:
;   rax: the string whose length is being calculated
; Returns:
;   rsi: the length
cstr~length:
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
cstr~print:
  push  rbp
  mov   rbp, rsp
                        ; Perserve the used regiser value
  push  rbx
  push  rsi

  call  cstr~length
  mov   rbx, rsi
  call  out~puts

  pop   rsi
  pop   rbx
  pop   rbp
  ret


; Args
;   rax: the string to be printed
; Returns
;   void
cstr~println:
  push  rbp
  mov   rbp, rsp
                        ; Perserve the used regiser value
  push  rdi
  push  rbx
  push  rcx
  push  rdx
  push  r8
  push  rsi

  call  cstr~length              ; Get the length with the string on rax

  lea   r8, [rax]               ; Get the start of the string
  add   r8, rsi                 ; Move to the end of the string
  mov   dl, 0xA
  mov   [r8], dl                ; Put the new line at the end of the string. This removes the null terminator but doesn't affect any other data

  mov   rbx, rsi
  add   rbx, 1                  ; Increase the length of the string since we added a character

  call  out~puts                ; Write the string to sys~out

  mov   dl, 0
  mov   [r8], dl                ; Re-add the null terminator

  pop   rsi
  pop   r8
  pop   rdx
  pop   rcx
  pop   rbx
  pop   rdi

  pop   rbp
  ret

%endif                          ; ifdef guard
