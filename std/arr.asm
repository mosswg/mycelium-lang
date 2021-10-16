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
;   rbx: index of the element
;   rcx: write value
; Returns
;   void
arr~set:
    mov     rsi, [rax]          ; Get size
    cmp     rsi, rbx            ; Validate Index
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

    mov     [rax+16], rcx       ; Offset by 16 to account for size and type variables

    pop     rax                 ; Perserve the pointer to the start of the array
    ret



; Args
;   rax: pointer to the array
;   rbx: new size
; Returns
;   rax: new pointer if needed
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
    mov     [rax], r12
    pop     r12
    pop     r11
    ret

; Args
;   rax: pointer to the array
;   rbx: the amount increase size by
; Returns
;   rax: new address pointer if needed
;   rsi: the new size
arr~increase_size:
    push    r11
    push    r12
    push    r13
    push    r14
    push    rax

    mov     r13, rbx            ; Store the increase amount
    lea     r12, [rax]          ; Store the address
    mov     r11, [rax+8]        ; Get the type

    mov     rax, r11
    call    type~sizeof         ; type.sizeof(rax)

    mov     rbx, rsi            ; rbx = size of each element
    mov     rax, [r12]          ; size
    mov     r14, rax
    add     rax, r13            ; rax = current_length + r13

    mov     rsi, rax

    mul     rbx                 ; rax = rax * rbx a.k.a the byte length of the array

    mov     rcx, rax

    mov     rax, r14

    mul     rbx

    mov     rbx, rax

    pop     rax
    push    rsi
    call    mem~reallocate
    mov     rbx, r14
    add     rbx, r13
    mov     [rax], rbx

    pop     rsi

    pop     r14
    pop     r13
    pop     r12
    pop     r11
    ret

; Args
;   rax: pointer to the array
;   rbx: value to push
; Returns
;   rax: the new pointer if needed
;
; Note: This function is not very performant. It is recommended that if you know how many elements you want to append you use resize then write each element after
arr~push:
    push    r9
    push    r10
    mov     r9, rbx             ; Store value to push in temp place
    mov     rbx, 1              ; Increase the size by 1
    call    arr~increase_size

    mov     rbx, rsi            ; Put the new size as the index for set
    sub     rbx, 1
    mov     rcx, r9             ; Put the value as the push value to write the end of the array
    call    arr~set             ; Set the value

    pop     r10
    pop     r9
    ret

; Args
;   rax: pointer to the array
; Returns
;   rsi: value
arr~pop:
    push    rbx
    push    rcx
    mov     rbx, [rax]          ; Load the size of the array into rbx

    sub     rbx, 1

    call    arr~get             ; rax.get(rbx)
    mov     [rax], rbx

    pop     rcx
    pop     rbx
    ret


%endif                          ; ifdef guard
