  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21

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
  sub   rbx, 1
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
  push  rbx
  push  rcx
  push  rdx
  push  r8
  push  rsi

  call  str~length              ; Get the length with the string on rax

  mov   rbx, rsi                ; Store in rbx because puts needs it there
  lea   r8, [rax]               ; Get the start of the string
  add   r8, rbx                 ; Move to the end of the string
  mov   cl, 0xA                 ; Store a new line character in cl
  mov   dl, [r8]                ; Store the current value at the end of the string
  mov   [r8], cl                ; Put the new line at the end of the array

  add   rbx, 1                  ; Increase the length of the string since we added a character

  call  out~puts                ; Write the string to sys~out

  mov   [r8], cl                ; Restore the old value at r8

  pop   rsi
  pop   r8
  pop   rdx
  pop   rcx
  pop   rbx

  pop   rbp
  ret
