  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21
%ifndef _Mycelium_std_str_
%define _Mycelium_std_str_

%include "std/sys.asm"
%include "std/out.asm"
%include "std/arr.asm"


  ;; String extends arr


; Args
;   void
; Returns
;   rsi: Pointer to string
str#new:
  mov   rax, 0
  mov   rbx, type#char

  call  arr#new

  ret

; Args
;   rax: String
; Return
;   rsi: length
str~length:
  mov rsi, [rax + arr#meta#user_size]
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
  push  rax

  call  str~length
  mov   rbx, rsi
  lea   rax, [rax + arr#meta_size]
  call  out~puts

  pop   rax
  pop   rsi
  pop   rbx
  pop   rbp
  ret


; Args
;   rax: the string to be printed
; Returns
;   void
str~println:
  push  rax
  call  str~print

  mov   rax, 0xA
  call  out~putc

  pop   rax
  ret

%endif                          ; ifdef guard
