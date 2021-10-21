    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_list_
%define _Mycelium_std_list_

%include "std/exception.asm"
%include "std/type.asm"
%include "std/mem.asm"

;;; TODO: Add get element type func


    ;; List archtecure
    ;; [memory size, user size, list type, amount of metadata, metadata..., type, data, type, data, etc...]

list#meta#mem_size               equ 0
list#meta#user_size              equ 8
list#meta#type                   equ 16
list#meta#meta_size              equ 24

list#global_allocate_stride:     equ 25*8        ; The memory size of an list is always a multiple of this + the type specific metadata size
list#type#global_meta:           equ 4*8

list#type#basic:                 equ 0
list#type#2d:                    equ 1
list#type#vector:                equ 2
list#type#string:                equ 3

list#global_element_size         equ 8

list#type#metadata_sizes:        db 0, 2*8, 4*8, 0



; Args
;   rax: list type
; Returns
;   rsi: size of the list metadata in bytes
list#metadata_size:
    push    rbx

    mov     rbx, list#type#metadata_sizes
    add     rbx, rax
    mov     bl, [rbx]
    movzx   rsi, bl

    pop     rbx
    ret

; Args
;   rax: pointer to the list
;   rbx: list type
;   rcx: list size
; Returns
;   void
list#populate_metadata:
    push    rax
    push    rsi
    mov     [rax+list#meta#user_size], rcx               ; User size

    mov     [rax+list#meta#type], rbx                    ; list Type

    mov     rcx, list#global_allocate_stride

    lea     rdx, [rax]
    mov     rax, rbx
    call    list#metadata_size

    mov     [rdx+list#meta#meta_size], rsi               ; Meta size

    add     rsi, list#type#global_meta

    add     rcx, rsi
    lea     rax, [rdx]

    mov     [rax+list#meta#mem_size], rcx                ; Memory Size

    pop     rsi
    pop     rax
    ret


; Args
;   rax: the type of the list
;   rbx: the size of the new list in elements
; Returns
;   rsi: pointer to the list
list#new:
    push    rcx

    mov     rcx, rbx
    mov     rbx, rax

    call    list#metadata_size               ; Put the total metadata size into rsi

    mov     rax, list#global_allocate_stride          ; Load the stride into rax

    add     rax, rsi                        ; Add the stride and type specific metadata size
    add     rax, list#type#global_meta       ; Add the global metadata size
    call    mem~allocate                    ; Create an array in memory with the size specified in global stride

    lea     rax, [rsi]
    call    list#populate_metadata

    pop     rcx
    ret

; Args
;   rax: pointer to the list
; Returns
;   void
list~del:
    mov     rbx, [rax]
    call    mem~deallocate
    ret

; Args
;   rax: pointer to the list
; Returns
;   rsi: amount of metadata in bytes
list~total_metadata:
    mov     rsi, [rax+list#meta#meta_size]
    add     rsi, list#type#global_meta
    ret

; Args
;   rax: pointer to the list
; Returns
;   rsi: pointer to the data after the metadata
list~addr_after_meta:
    push    rax
    mov     rsi, [rax+list#meta#meta_size]
    add     rsi, list#type#global_meta
    add     rax, rsi
    mov     rsi, rax
    pop     rax
    ret

; Args
;   rax: pointer to the list
; Returns
;   void
list~print_string:
    .string:    db ", ", 0x0
list~print:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    r9
    push    r10
    mov     r10, rax
    mov     r9, list#global_element_size*2 ; Move the size to a usable location


    mov     rax, [r10+list#meta#user_size]          ; Get length

    mul     r9                      ; Multiple by the size of element stride

    lea     rcx, [r10+list#meta#meta_size]

    add     rcx, rax                ; Get the end address

    mov     rax, '['
    call    out~putc

    lea     r10, [r10+list#type#global_meta]       ; Offset by to the amount of metadata

    mov     rsi, [r10]              ; Get the amount of metadata

    add     r10, rsi                ; Add metadata offset to

    add     rcx, rsi                ; Offset the end address too

    mov     rax, [r10+8]
    mov     rbx, [r10]
    call    type~print

    add     r10, r9                 ; Increment the index

    .loop:
        mov     rax, list~print_string
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
    pop     rax                 ; Perserve the pointer to the start of the list
    ret

; Args
;   rax: pointer to the list
; Returns
;   void
list~println:
    push    rax
    call    list~print

    mov     rax, 0xA
    call    out~putc

    pop     rax                 ; Perserve the pointer to the start of the list
    ret


; Args
;   rax: pointer to the list
; Returns
;   void
list~printn:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    r9
    push    r10
    mov     r10, rax
    mov     r9, list#global_element_size            ; Move the size to a usable location

    lea     rcx, [r10]

    mov     rax, [r10]                              ; Get length

    add     rcx, rax                                ; Get the end address


    lea     r10, [r10+list#meta#meta_size]          ; Offset by to the amount of metadata
    lea     rcx, [rcx+list#meta#meta_size]          ; Offset the end address too

    mov     rsi, [r10]                              ; Get the amount of metadata

    add     r10, rsi                                ; Add metadata offset to

    add     rcx, rsi                                ; Offset the end address too


    .loop:
        mov     rax, [r10+8]
        mov     rbx, [r10]
        call    type~print

        mov     rax, 0xA
        call    out~putc

        add     r10, r9                             ; Increment the index
    .loop_check:
        cmp     r10, rcx
        jl     .loop

    pop     r10
    pop     r9
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax                     ; Perserve the pointer to the start of the list
    ret




; Args
;   rax: pointer to the list
; Returns
;   rsi: length of the list
list~len:
    mov     rsi, [rax+list#meta#user_size]
    ret


; Args
;   rax: pointer to the list
;   rbx: index of the element
; Returns
;   rsi: element
list~get:
    mov     rsi, [rax+list#meta#user_size]          ; Get the length
    cmp     rsi, rbx            ; Validate index
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax

    call    list~addr_after_meta
    shr     rbx, 1

    add     rsi, rbx
    mov     rsi, [rsi+list#global_element_size]

    pop     rax                 ; Perserve the pointer to the start of the list
    ret


; Args
;   rax: pointer to the list
;   rbx: index of the element
; Returns
;   rsi: element type
list~type_at:
    mov     rsi, [rax+list#meta#user_size]          ; Get the length
    cmp     rsi, rbx            ; Validate index
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax

    call    list~addr_after_meta
    shr     rbx, 1

    add     rsi, rbx
    mov     rsi, [rsi]

    pop     rax                 ; Perserve the pointer to the start of the list
    ret


; Args
;   rax: pointer to the list
;   rbx: index of the element
;   rcx: write value
;   rdx: type
; Returns
;   void
list~set:

    mov     rsi, [rax+list#meta#user_size]          ; Get the length
    cmp     rsi, rbx                                ; Validate index
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax

    call    list~addr_after_meta

    shl     rbx, 4                                  ; Index * 2 to compensate for types and then index * 8 to compensate for the size of each element

    add     rsi, rbx
    mov     [rsi], rdx
    mov     [rsi+list#global_element_size], rcx

    shr     rbx, 4                                  ; Reset rbx to orignal value

    pop     rax                                     ; Perserve the pointer to the start of the list
    ret



; Args
;   rax: pointer to the list
;   rbx: new size
; Returns
;   rax: new pointer if needed
list~resize:
    push    r9
    push    rdx
    push    rbx
    push    rax

    mov     r9, [rax+list#meta#mem_size]
    mov     rdx, rbx
    shl     rdx, 3              ; Multiply by 8, the size of one element

    pop     rax
    call    list~total_metadata
    add     rdx, rsi

    cmp     rdx, r9
    pop     rbx
    jl      .good_size

    push    r10
    mov     r10d, 100
    push    rbx
    push    rcx
    push    rax

    mov     rax, rbx
    shl     rax, 3              ; Multiply by 3 to get the byte size
    mov     rdx, 0
    div     r10d

    add     rax, 1              ; Add one to result because integer division always rounds down when there is a remainder and it's more of an issue to have too little than too much

    mul     r10d                 ; Get the new size in an multiple of 100

    add     rax, rsi            ; Add the amount of metadata

    mov     rcx, rax
    pop     rax
    mov     rbx, [rax+list#meta#mem_size]

    mov     [rax+list#meta#mem_size], rcx

    call    mem~reallocate

    pop     rcx
    pop     rbx
    pop     r10
    .good_size:


    mov     [rax+list#meta#user_size], rbx

    pop     rdx
    pop     r9
    ret

; Args
;   rax: pointer to the list
;   rbx: the amount increase size by
; Returns
;   rax: new address pointer if needed
;   rsi: the new size
list~increase_size:
    push    rdx
    push    rcx

    mov     rcx, [rax]
    mov     rdx, [rax+list#meta#user_size]

    call    list~total_metadata
    sub     rcx, rsi

    push    rax
    push    rbx

    mov     rax, rcx
    mov     rbx, list#global_element_size

    mul     rbx
    mov     rcx, rax

    pop     rbx
    pop     rax

    add     rdx, rsi

    cmp     rdx, rcx
    jl      .enough_mem

    push    rbx
    mov     rbx, rcx
    add     rcx, list#global_allocate_stride
    call    mem~reallocate

    .enough_mem:

    add     [rax+list#meta#user_size], rbx

    mov     rsi, [rax+list#meta#user_size]

    pop     rcx
    pop     rdx
    ret

; Args
;   rax: pointer to the list
;   rbx: value to push
;   rcx: type
; Returns
;   rax: the new pointer if needed
list~push:
    push    r9
    push    r10
    mov     r9, rbx             ; Store value to push in temp place
    mov     rbx, 1              ; Increase the size by 1
    call    list~increase_size

    mov     rbx, rsi            ; Put the new size as the index for set
    sub     rbx, 1
    mov     rdx, rcx
    mov     rcx, r9             ; Put the value as the push value to write the end of the list
    call    list~set             ; Set the value

    pop     r10
    pop     r9
    ret

; Args
;   rax: pointer to the list
; Returns
;   rsi: value
list~pop:
    push    rbx
    push    rcx
    mov     rbx, [rax]          ; Load the size of the list into rbx

    sub     rbx, 1

    call    list~get             ; rax.get(rbx)
    mov     [rax], rbx

    pop     rcx
    pop     rbx
    ret


%endif                          ; ifdef guard
