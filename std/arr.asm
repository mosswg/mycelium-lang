    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_arr_
%define _Mycelium_std_arr_

%include "std/exception.asm"
%include "std/type.asm"
%include "std/mem.asm"

;;; TODO: Add get element type func


    ;; Array archtecure
    ;; [memory size, user size, array type, amount of metadata, metadata..., data type, data, data type, data, etc...]

arr#meta#mem_size               equ 0
arr#meta#user_size              equ 8
arr#meta#type                   equ 16
arr#meta#meta_size              equ 24

arr#global_allocate_stride:     equ 25*8        ; The memory size of an array is always a multiple of this + the type specific metadata size
arr#type#global_meta:           equ 4*8

arr#type#basic:                 equ 0
arr#type#2d:                    equ 1
arr#type#vector:                equ 2
arr#type#string:                equ 3

arr#global_element_size         equ 8

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
    push    rsi
    mov     [rax+arr#meta#user_size], rcx               ; User size

    mov     [rax+arr#meta#type], rbx                    ; Array Type

    mov     rcx, arr#global_allocate_stride

    lea     rdx, [rax]
    mov     rax, rbx
    call    arr#metadata_size

    mov     [rdx+arr#meta#meta_size], rsi               ; Meta size

    add     rsi, arr#type#global_meta

    add     rcx, rsi
    lea     rax, [rdx]

    mov     [rax+arr#meta#mem_size], rcx                ; Memory Size

    pop     rsi
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

    mov     rax, arr#global_allocate_stride          ; Load the stride into rax

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
;   rsi: amount of metadata in bytes
arr~total_metadata:
    mov     rsi, [rax+arr#meta#meta_size]
    add     rsi, arr#type#global_meta
    ret

; Args
;   rax: pointer to the array
; Returns
;   rsi: pointer to the data after the metadata
arr~addr_after_meta:
    push    rax
    mov     rsi, [rax+arr#meta#meta_size]
    add     rsi, arr#type#global_meta
    add     rax, rsi
    mov     rsi, rax
    pop     rax
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
    mov     r10, rax
    mov     r9, arr#global_element_size*2 ; Move the size to a usable location


    mov     rax, [r10+arr#meta#user_size]          ; Get length

    mul     r9                  ; Multiple by the size of element stride

    lea     rcx, [r10+arr#meta#meta_size]

    add     rcx, rax            ; Get the end address

    mov     rax, '['
    call    out~putc

    lea     r10, [r10+arr#type#global_meta]       ; Offset by to the amount of metadata

    mov     rsi, [r10]          ; Get the amount of metadata

    add     r10, rsi            ; Add metadata offset to

    add     rcx, rsi            ; Offset the end address too

    mov     rax, [r10+8]
    mov     rbx, [r10]
    call    type~print

    add     r10, r9            ; Increment the index

    .loop:
        mov     rax, arr~print_string
        mov     rbx, 2
        call    out~puts


        mov     rax, [r10+8]
        mov     rbx, [r10]
        call    type~print

        add     r10, r9            ; Increment the index
    .loop_check:
        cmp     r10, rcx
        jl     .loop


    mov     rax, ']'
    call    out~putc

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
    call    arr~print

    mov     rax, 0xA
    call    out~putc

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
    mov     r9, arr#global_element_size ; Move the size to a usable location

    lea     rcx, [r10]

    mov     rax, [r10]          ; Get length

    add     rcx, rax            ; Get the end address


    lea     r10, [r10+arr#meta#meta_size]       ; Offset by to the amount of metadata
    lea     rcx, [rcx+arr#meta#meta_size]       ; Offset the end address too

    mov     rsi, [r10]          ; Get the amount of metadata

    add     r10, rsi            ; Add metadata offset to

    add     rcx, rsi            ; Offset the end address too


    .loop:
        mov     rax, [r10+8]
        mov     rbx, [r10]
        call    type~print

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
    mov     rsi, [rax+arr#meta#user_size]
    ret


; Args
;   rax: pointer to the array
;   rbx: index of the element
; Returns
;   rsi: element
arr~get:
    mov     rsi, [rax+arr#meta#user_size]          ; Get the length
    cmp     rsi, rbx            ; Validate index
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax

    call    arr~addr_after_meta
    shr     rbx, 1

    add     rsi, rbx
    mov     rsi, [rsi+arr#global_element_size]

    pop     rax                 ; Perserve the pointer to the start of the array
    ret


; Args
;   rax: pointer to the array
;   rbx: index of the element
;   rcx: write value
;   rdx: type
; Returns
;   void
arr~set:

    mov     rsi, [rax+arr#meta#user_size]          ; Get the length
    cmp     rsi, rbx            ; Validate index
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax

    call    arr~addr_after_meta

    shl     rbx, 4              ; Index * 2 to compensate for types and then index * 8 to compensate for the size of each element

    add     rsi, rbx
    mov     [rsi], rdx
    mov     [rsi+arr#global_element_size], rcx

    shr     rbx, 4              ; Reset rbx to orignal value

    pop     rax                 ; Perserve the pointer to the start of the array
    ret



; Args
;   rax: pointer to the array
;   rbx: new size
; Returns
;   rax: new pointer if needed
arr~resize:
    push    rdx
    push    rcx

    mov     rcx, [rax]
    mov     rdx, [rax+arr#meta#user_size]

    call    arr~total_metadata
    sub     rcx, rsi

    push    rax
    push    rbx

    mov     rax, rcx
    mov     rbx, arr#global_element_size

    mul     rbx
    mov     rcx, rax

    pop     rbx
    pop     rax

    add     rdx, rsi

    cmp     rdx, rcx
    jl      .enough_mem

    push    rbx
    mov     rbx, rcx
    add     rcx, arr#global_allocate_stride
    call    mem~reallocate

    .enough_mem:

    mov     [rax+arr#meta#user_size], rbx

    pop     rcx
    pop     rdx
    ret
    ret

; Args
;   rax: pointer to the array
;   rbx: the amount increase size by
; Returns
;   rax: new address pointer if needed
;   rsi: the new size
arr~increase_size:
    push    rdx
    push    rcx

    mov     rcx, [rax]
    mov     rdx, [rax+arr#meta#user_size]

    call    arr~total_metadata
    sub     rcx, rsi

    push    rax
    push    rbx

    mov     rax, rcx
    mov     rbx, arr#global_element_size

    mul     rbx
    mov     rcx, rax

    pop     rbx
    pop     rax

    add     rdx, rsi

    cmp     rdx, rcx
    jl      .enough_mem

    push    rbx
    mov     rbx, rcx
    add     rcx, arr#global_allocate_stride
    call    mem~reallocate

    .enough_mem:

    add     [rax+arr#meta#user_size], rbx

    mov     rsi, [rax+arr#meta#user_size]

    pop     rcx
    pop     rdx
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
