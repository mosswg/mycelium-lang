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
;   void
arr~print_string:
    .string:    db ", ", 0x0
arr~print:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    r9
    push    r10
    push    r11
    mov     r10, rax
    mov     r11, rbx
    mov     rdx, [r10+8]        ; Get the type into rax
    mov     rax, rdx
    call    type~sizeof         ; Get the size of each element into rsi
    mov     r9, rsi             ; Move the size to a usable location

    lea     rcx, [r10]

    mov     rax, [r10]          ; Get length
    mul     r9

    add     rcx, rax            ; Get the end address

    mov     rax, '['
    call    out~putc

    lea     r10, [r10+16]       ; Offset by 16 to account for size and type variables
    lea     rcx, [rcx+16]       ; Offset the end address too

    mov rax, [r10]
    mov rbx, rdx
    call type~print

    add     r10, r9            ; Increment the index

    .loop:
        mov     rax, arr~print_string
        mov     rbx, 2
        call    out~puts


        mov rax, [r10]
        mov rbx, rdx
        call type~print

        add     r10, r9            ; Increment the index
    .loop_check:
        cmp     r10, rcx
        jl     .loop


    mov     rax, ']'
    call    out~putc

    pop     r11
    pop     r10
    pop     r9
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax                 ; Perserve the pointer to the start of the array
    ret

; Args
;   rax: pointer to the array
; Returns
;   void
arr~println:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    r9
    push    r10
    push    r11
    mov     r10, rax
    mov     r11, rbx
    mov     rdx, [r10+8]        ; Get the type into rax
    mov     rax, rdx
    call    type~sizeof         ; Get the size of each element into rsi
    mov     r9, rsi             ; Move the size to a usable location

    lea     rcx, [r10]

    mov     rax, [r10]          ; Get length
    mul     r9

    add     rcx, rax            ; Get the end address

    mov     rax, '['
    call    out~putc

    lea     r10, [r10+16]       ; Offset by 16 to account for size and type variables
    lea     rcx, [rcx+16]       ; Offset the end address too

    mov rax, [r10]
    mov rbx, rdx
    call type~print

    add     r10, r9            ; Increment the index

    .loop:
        mov     rax, arr~print_string
        mov     rbx, 2
        call    out~puts


        mov rax, [r10]
        mov rbx, rdx
        call type~print

        add     r10, r9            ; Increment the index
    .loop_check:
        cmp     r10, rcx
        jl     .loop


    mov     rax, ']'
    call    out~putc
    mov     rax, 0xA
    call    out~putc

    pop     r11
    pop     r10
    pop     r9
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax                 ; Perserve the pointer to the start of the array
    ret


; Args
;   rax: pointer to the array
; Returns
;   void
arr~printn:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    r9
    push    r10
    mov     r10, rax
    mov     rdx, [r10+8]        ; Get the type into rax
    mov     rax, rdx
    call    type~sizeof         ; Get the size of each element into rsi
    mov     r9, rsi             ; Move the size to a usable location

    lea     rcx, [r10]

    mov     rax, [r10]          ; Get length
    mul     r9

    add     rcx, rax            ; Get the end address



    lea     r10, [r10+16]       ; Offset by 16 to account for size and type variables
    lea     rcx, [rcx+16]       ; Offset the end address too

    .loop:
        mov rax, [r10]
        mov rbx, rdx
        call type~print

        mov     rax, 0xA
        call    out~putc

        add     r10, r9            ; Increment the index
    .loop_check:
        cmp     r10, rcx
        jl     .loop

    pop     r10
    pop     r9
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax                 ; Perserve the pointer to the start of the array
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

; Args
;   rax: pointer to the array
;   rbx: new size
; Returns
;   rsi: new pointer if needed
arr~resize:
    push    r11
    push    r12
    push    rax
    mov     rcx, rbx
    mov     r12, rbx
    mov     rbx, [rax]
    mov     rax, [rax+8]        ; Get type
    call    type~sizeof
    mov     rax, rcx
    mul     rsi
    mov     rcx, rax
    mov     rax, rbx
    mul     rsi
    mov     rbx, rax
    pop     rax
    call    mem~reallocate
    mov     [rsi], r12
    pop     r12
    pop     r11
    ret


%endif                          ; ifdef guard
