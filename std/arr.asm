    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _mxs_std_arr_
%define _mxs_std_arr_

%include "std/exception.asm"
%include "std/type.asm"
%include "std/mem.asm"

; Args
;   rax: the size of the new array in elements
;   rbx: the type of the elements
; Returns
;   rsi: pointer to the array
arr~new:
    push    rcx
    push    rax

    mov     rax, rbx
    call    type~sizeof
    pop     rax
    push    rax

    mul     rsi
    add     rsi, 16             ; Add Two 8-byte numbers for array size and size of each element

    call    mem~allocate

    pop     rcx

    mov     [rsi], rcx          ; Store the size of the array
    mov     [rsi+8], rbx        ; Store the type of the array

    pop     rcx
    ret

; Args
;   rax: pointer to the array
; Returns
;   void
arr~del:
    mov     rbx, [rax]
    call    mem~deallocate
    ret


; Args
;   rax: pointer to the array
; Returns
;   rsi: length of the array
arr~len:
    mov     rsi, [rax]
    ret


; Args
;   rax: pointer to the array
;   rbx: index of the element
; Returns
;   rsi: element
arr~get:
    mov     rsi, [rax]
    cmp     rsi, rbx
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax
    push    rax
    mov     rax, [rax+8]        ; Get the type into rax
    call    type~sizeof         ; Get the size of each element into rsi

    mov     rax, rbx
    mul     rsi                 ; Get the byte index of the element
    mov     rsi, rax
    pop     rax
    add     rax, rsi            ; Get move the index minus 16


    mov     rsi, [rax+16]       ; Offset by 16 to account for size and type variables



    pop     rax                 ; Perserve the pointer to the start of the array
    ret



%endif                          ; ifdef guard
