  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21

%ifndef _mxs_std_int_
%define _mxs_std_int_

%include "std/out.asm"

; Args
;   rax: out string
;   rbx: number
; Return
;   rsi: number of digits
int~to_string:
  push  rbp
  mov   rbp, rsp


  push  rdx             ; Save the registers we don't want to affect after the function
  push  rcx
  push  r8
  push  r9
  push  r10
  push  r11

  mov   r9, rax         ; Move the out array to a place we don't need for other functions
  mov   r8, rbx
  mov   rax, rbx
  call  int~digits
  push  rsi
  
  cmp   rsi, 1
  jne   .multidigit
  .singledigit:
    lea rbx, [r9]
    mov r11b, 48
    add r11b, al
    mov [rbx], r11b
    jmp .return
  .multidigit:
  sub   rsi, 1

  mov rcx, 1
  mov r10, 10


  jmp .loop_check
  .loop:
    xor rdx, rdx
    div r10

    lea rbx, [r9]
    add rbx, rsi
    mov r11b, 48
    add r11b, dl
    mov [rbx], r11b

    sub rsi, 1
  .loop_check:
    cmp rsi, 0
    jge .loop



  .return:
  mov rax, r9
  mov rbx, r8
  pop   rsi
  pop   r11
  pop   r10
  pop   r9
  pop   r8
  pop   rcx
  pop   rdx
  pop   rbp
  ret

; Args
;   rax: the number to be printed
; Returns
;   void
int~print:
  .debug:
  push  rbp
  mov   rbp, rsp
  push  rax
  mov   rbx, rax
  mov   rax, rsp
  call  int~to_string
  mov   rax, rsp
  mov   rbx, rsi
  call  out~puts
  pop   rax
  pop   rbp
  ret



; Args
;   rax: the number to be printed
; Returns
;   void
int~println:
  .debug:
  push  rbp
  mov   rbp, rsp

  push  rax
  push  rsi

  mov   rbx, rax
  mov   rax, rsp
  call  int~to_string

  lea   rbx, [rsp]
  add   rbx, rsi
  mov   al, 0xA
  mov   [rbx], al

  mov   rax, rsp
  mov   rbx, rsi
  add   rbx, 1
  call  out~puts

  pop   rsi
  pop   rax
  pop   rbp
  ret

; Args
;   rax: number
; Returns
;   rsi: number of digits
int~digits:
  push  rbp
  mov   rbp, rsp
  
  push  rax
  push  rcx
  push  rdx

  mov   rsi, 0
  mov   ecx, 10

  jmp .loop_check
  .loop:
    xor edx, edx
    div ecx
    add rsi, 1
  .loop_check:
    cmp rax, 1
    jge .loop

  pop   rdx
  pop   rcx
  pop   rax
  pop   rbp
  ret


%endif                          ; ifdef guard
