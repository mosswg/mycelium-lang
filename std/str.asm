  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21
%ifndef _Mycelium_std_str_
%define _Mycelium_std_str_

%include "std/sys.asm"
%include "std/out.asm"
%include "std/arr.asm"
%include "std/cstr.asm"


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
;   rax: c string
; Returns
;   rsi: Pointer to string
str#new_cs:
  push  r8                      ; c string
  push  r9                      ; Temporary Place for String
  push  r10                     ; Counter
  push  r11                     ; Current char in loop

  mov   r8, rax

  mov   rax, 0
  mov   rbx, type#char

  call  arr#new
  mov   r9, rsi

  xor   r10, r10


  ;; Copy values over
  jmp   .loop_check
  .loop:
      mov   rax, r9
      movzx rbx, r11b
      call  arr~push
      add   r10, 1
  .loop_check:
      mov   r11b, [r8+r10]
      cmp   r11b, 0x0
      jne   .loop

  pop   r11
  pop   r10
  pop   r9
  pop   r8
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
;   rax: string
;   rbx: compare value
; Returns
;   zf: equality
str~eq:
  push  rcx

  cmp   rbx, 256
  jl    .str_eq_char

  .str_eq_char:
  mov   rcx, [rax + arr#meta#user_size]

  cmp   rcx, 1
  jne   .return

  mov   rcx, rbx
  mov   rbx, 1
  call  arr~get

  cmp   rcx, rsi
  jmp   .return

  .str_eq_str:
  call  arr~compare

  .return:

  pop   rcx
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

; Args
;   rax: string
; Returns
;   zf: is int
str~is_int:
  push    r8                    ; counter
  push    r9                    ; '0'
  push    r10                   ; '9'
  push    r11                   ; str
  push    r12                   ; str size

  xor     r9, r9
  mov     r9b, '0'
  xor     r10, r10
  mov     r10b, '9'
  lea     r11, [rax]
  mov     r12, [rax + arr#meta#user_size]

  xor     r8, r8

  .loop:
    lea   rax, [r11]
    mov   rbx, r8
    call  arr~get

    cmp   si, r9w
    jl    .non_num
    cmp   si, r10w
    jg    .non_num

    add   r8, 1
  .loop_check:
    cmp   r8, r12
    jl    .loop

  jmp   .num
  .non_num:
  mov   r8, 1
  cmp   r8, 0
  jmp   .return

  .num:
  mov   r8, 1
  cmp   r8, 1

  .return:
  pop     r12
  pop     r11
  pop     r10
  pop     r9
  pop     r8
  ret

; Args
;   rax: string
; Returns
;   rsi: int form
str~to_int:
  push  r8                            ; string
  push  r9                            ; len
  push  r10                           ; counter
  mov   sil, [rax+arr#meta_size]      ; out
  sub   sil, '0'
  mov   r10, 1

  mov   r8, rax
  mov   r9, [r8+arr#meta#user_size]


  jmp   .loop_check
  .loop:
    mov   rax, rsi
    mov   rbx, 10
    mul   rbx

    mov   rsi, rax

    xor   rax, rax
    mov   al, [r8+arr#meta_size + r10]
    sub   al, '0'
    add   rsi, rax
    add   r10, 1
  .loop_check:
    cmp   r10, r9
    jl    .loop

  pop   r10
  pop   r9
  pop   r8
  ret



; Args
;   rax: string
;   rbx: char
; Returns
;   zf: starts with char
str~starts_with_c:
  push  r8

  mov   r8b, [rax+arr#meta_size]
  cmp   r8b, bl

  pop   r8
  ret


; Args
;   rax: string
;   rbx: char
; Returns
;   zf: ends with char
str~ends_with_c:
  push  r8
  push  r9

  mov   r9, [rax + arr#meta#user_size]
  mov   r8b, [rax + r9 + arr#meta_size - 1]

  cmp   r8b, bl

  pop   r8
  pop   r9
  ret



; Args
;   rax: string
;   rbx: char
; Returns
;   zf: if wrapped
str~wrapped_with_c:
  call  str~starts_with_c
  jne   .return
  call  str~ends_with_c

  .return:
  ret

; Args
;   rax: string
;   rbx: char to split on
; Return
;   rsi: Array with split strings
str~split:
    push    r8                  ; Split counter
    push    r9                  ; Output array
    push    r10                 ; Split char
    push    r11                 ; Original Array Pointer
    push    r12                 ; Counter
    push    r13                 ; Pointer to after the metadata
    push    r14                 ; Size of the array

    mov     r10, rbx
    lea     r11, [rax]
    mov     r14, [rax + arr#meta#user_size]
    lea     r13, [rax + arr#meta_size]
    xor     r12, r12
    xor     r8, r8

    mov     rax, 0
    mov     rbx, type#string

    call    arr#new

    lea     r9, [rsi]

    call    str#new

    lea     rax, [r9]
    lea     rbx, [rsi]
    call    arr~push

  jmp   .loop_check
  .loop:
      xor     rcx, rcx
      mov     cl, [r13+r12]    ; Current char

      cmp     rcx, r10
      jne     .non_split
      .split:
        call  str#new

        lea     rax, [r9]
        lea     rbx, [rsi]
        call    arr~push

        add   r8, 1
        add   r12, 1
        jmp   .loop_check
      .non_split:
        lea   rax, [r9]
        mov   rbx, r8
        call  arr~get

        lea   rax, [rsi]
        mov   rbx, rcx

        call arr~push

        mov   rcx, rax
        mov   rbx, r8
        lea   rax, [r9]

        call  arr~set

        add   r12, 1
  .loop_check:
      cmp     r12, r14
      jl      .loop

    lea     rsi, [r9]

    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret

; Args
;   rax: string
;   rbx: tuple of split chars
; Return
;   rsi: Array with split strings
str~split_t:
    push    r8                  ; Split counter
    push    r9                  ; Output array
    push    r10                 ; Split tuple
    push    r11                 ; Original Array Pointer
    push    r12                 ; Counter
    push    r13                 ; Pointer to after the metadata
    push    r14                 ; Size of the array

    mov     r10, rbx
    lea     r11, [rax]
    mov     r14, [rax + arr#meta#user_size]
    lea     r13, [rax + arr#meta_size]
    xor     r12, r12
    xor     r8, r8

    mov     rax, 0
    mov     rbx, type#string

    call    arr#new

    lea     r9, [rsi]

    call    str#new

    lea     rax, [r9]
    lea     rbx, [rsi]
    call    arr~push

  jmp   .loop_check
  .loop:
      xor     rcx, rcx
      mov     cl, [r13+r12]    ; Current char

      mov     rax, r10
      movzx   rbx, cl
      call    tuple~contains    ; Check if the char is in the tuple
      jne     .non_split
      .split:
        push    rcx               ; Store the current char

        lea     rax, [r9]       ; Get the out array
        mov     rbx, r8         ; Get the current index
        call    arr~get         ; Get the string at that index

        mov     rax, [rsi+arr#meta#user_size] ; Get the size of the previous string

        cmp     rax, 0          ; If the string length is 0 (two split characters in a row)
        jne      .full_split    ; We want to do a pseudo split

        lea     rax, [r9]
        call    arr~pop
        mov     rdi, rsi
        sub     r8, 1
        jmp     .pseudo_split

        .full_split:
        call    str#new           ; Create a new string

        lea     rdi, [rsi]

        .pseudo_split:
        lea     rax, [rsi]      ; Get the new string
        mov     rbx, [rsp]      ; Move rcx (on the stack) into rbx
        call    arr~push

        lea     rbx, [rdi]      ; Get the previously created string
        lea     rax, [r9]       ; Get the out array
        call    arr~push        ; Add the string to the out array

        add     r8, 1

        call    str#new
        lea     rax, [r9]
        lea     rbx, [rsi]
        call    arr~push

        add   r8, 1             ; Add one to the number of splits
        add   r12, 1
        pop   rcx               ; Get back the current char
        jmp   .loop_check
      .non_split:
        lea   rax, [r9]         ; Get the out array
        mov   rbx, r8           ; Get the current split
        call  arr~get           ; Get the current split string


      .write:
        lea   rax, [rsi]
        mov   rbx, rcx

        call arr~push

        mov   rcx, rax
        mov   rbx, r8
        lea   rax, [r9]

        call  arr~set

        add   r12, 1
  .loop_check:
      cmp     r12, r14
      jl      .loop

    lea     rsi, [r9]

    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret

%endif                          ; ifdef guard
