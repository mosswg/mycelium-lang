    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _mxs_std_arr_
%define _mxs_std_arr_

%include "std/exception.asm"
%include "std/type.asm"
%include "std/mem.asm"

    ;; Array archtecure
    ;; [memory size, user size, array type, amount of metadata, metadata..., data type, data, data type, data, etc...]

arr#meta#mem_size               equ 0
arr#meta#user_size              equ 8
arr#meta#type                   equ 16
arr#meta#meta_size              equ 24

arr#global_stride:              equ 25*8        ; The memory size of an array is always a multiple of this + the type specific metadata size
arr#type#global_meta:           equ 4*8

arr#type#basic:                 equ 0
arr#type#2d:                    equ 1
arr#type#vector:                equ 2
arr#type#string:                equ 3

arr#type#metadata_sizes:        db 0, 2*8, 4*8, 0



; Args
;   rax: array type
; Returns
;   rsi: size of the array metadata in bytes
arr#metadata_size:
    push    rbx

    mov     rbx, arr#type#metadata_sizes
    add     rbx, rax
    mov     bl, [rbx]
    movzx   rsi, bl


    pop     rbx
    ret

; Args
;   rax: array pointer
;   rbx: array type
;   rcx: array size
; Returns
;   void
arr#populate_metadata:
    push    rax
    mov     [rax+arr#meta#user_size], rcx               ; User size

    mov     [rax+arr#meta#type], rbx                    ; Array Type

    mov     rcx, arr#global_stride

    lea     rdx, [rax]
    mov     rax, rbx
    call    arr#metadata_size

    mov     [rdx+arr#meta#meta_size], rsi               ; Meta size

    add     rsi, arr#type#global_meta

    add     rcx, rsi
    lea     rax, [rdx]

    mov     [rax+arr#meta#mem_size], rcx                ; Memory Size

    pop     rax
    ret


; Args
;   rax: the type of the array
;   rbx: the size of the new array in elements
; Returns
;   rsi: pointer to the array
arr#new:
    push    rcx

    mov     rcx, rbx
    mov     rbx, rax

    call    arr#metadata_size               ; Put the total metadata size into rsi

    mov     rax, arr#global_stride          ; Load the stride into rax

    add     rax, rsi                        ; Add the stride and type specific metadata size
    add     rax, arr#type#global_meta       ; Add the global metadata size
    call    mem~allocate                    ; Create an array in memory with the size specified in global stride

    lea     rax, [rsi]
    call    arr#populate_metadata

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
