    ;; Author:  Moss Gallagher
    ;; Date:    21-Oct-21

%ifndef _Mycelium_std_tuple_
%define _Mycelium_std_tuple_

%include "std/exception.asm"
%include "std/type.asm"
%include "std/mem.asm"

    ;; Tuple archtecure
    ;; [size, type, data, type, data, etc...]

tuple#element_size:             equ 8

tuple#meta#size                 equ 0*8          ; The size of the array in memory. This is different to user size because we're lying to the user

tuple#meta_size:                equ 1*8



; Args
;   rax: tuple pointer
;   rbx: size
; Returns
;   void
tuple#populate_metadata:
    mov     [rax+tuple#meta#size], rbx
    ret


; Args
;   rax: size
; Returns
;   rsi: pointer to the tuple
tuple#new:

    mov     rbx, rax

    shl     rax, 4                          ; Multiply rax (the size) by 16
    call    mem~allocate                    ; Create an array in memory with the size specified in global stride

    lea     rax, [rsi]
    call    tuple#populate_metadata

    ret



; Args
;   rax: size
;   stack: The elements to add in the format [type, data, type, data, ...]
; Returns
;   rsi: pointer to the tuple
tuple#new_ca:
    lea     rsi, [rsp+8]
    push    r8                  ; Loop counter
    push    r9                  ; Size
    push    r10                 ; Out ptr
    push    r11                 ; Stack

    mov     r11, rsi

    mov     r9, rax

    shl     rax, 4                          ; Multiply rax (the size) by 16
    call    mem~allocate                    ; Create an array in memory with the size specified in global stride

    lea     r10, [rsi]

    lea     rax, [rsi]
    mov     rbx, r9
    call    tuple#populate_metadata

    xor     r8, r8



    jmp     .loop_check
    .loop:
        mov     rax, r8
        shl     rax, 4

        mov     rdx, [r11 + rax]
        mov     rcx, [r11 + rax + tuple#element_size]
        mov     rbx, r8
        mov     rax, r10

    
        call    tuple~set

        add     r8, 1
    .loop_check:
        cmp     r8, r9
        jl      .loop

    mov     rsi, r10

    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret



; Args
;   rax: pointer to the tuple
; Returns
;   void
tuple~del:
    mov     rbx, [rax+tuple#meta#size]
    shl     rbx, 4              ; Multiply by 16
    call    mem~deallocate
    ret


; Args
;   rax: pointer to the tuple
; Returns
;   void
tuple#print_string:
    .string:    db ", ", 0x0
tuple~print:
    push    r8                  ; Loop counter
    push    r9                  ; Size
    push    r10                 ; Out ptr

    xor     r8, r8

    mov     r9, [rax+tuple#meta#size]

    lea     r10, [rax]

    mov     rax, '('
    call    out~putc


    mov     rbx, 0
    lea     rax, [r10]

    call    tuple~get

    mov     rax, rsi
    mov     rbx, rdi

    call    type~print

    mov     r8, 1

    jmp     .loop_check
    .loop:
        mov     rax, tuple#print_string
        call    cstr~print

        mov     rbx, r8
        lea     rax, [r10]

        call    tuple~get

        mov     rax, rsi
        mov     rbx, rdi

        call    type~print

        add     r8, 1
    .loop_check:
        cmp     r8, r9
        jl      .loop

    mov     rax, ')'
    call    out~putc

    mov     rsi, r10

    pop     r10
    pop     r9
    pop     r8
    ret

; Args
;   rax: pointer to the tuple
; Returns
;   void
tuple~println:
    push    rax

    call    tuple~print

    mov     rax, 0xA
    call    out~putc

    pop     rax
    ret

; Args
;   rax: pointer to the tuple
; Returns
;   rsi: length of the tuple
tuple~length:
    mov     rsi, [rax+tuple#meta#size]
    ret


; Args
;   rax: pointer to the tuple
;   rbx: index of the element
; Returns
;   rsi: element
;   rdi: type
tuple~get:
    mov     rsi, [rax+tuple#meta#size]
    cmp     rsi, rbx
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rbx

    shl     rbx, 4
    add     rbx, tuple#meta_size
    mov     rsi, [rax + rbx + tuple#element_size]
    mov     rdi, [rax + rbx]

    pop     rbx
    ret

; Args
;   rax: pointer to the tuple
;   rbx: index of the element
;   rcx: write value
;   rdx: write type
; Returns
;   void
tuple~set:
    mov     rsi, [rax+tuple#meta#size]
    cmp     rsi, rbx
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rbx

    shl     rbx, 4
    add     rbx, tuple#meta_size
    mov     [rax + rbx], rdx
    mov     [rax + rbx + tuple#element_size], rcx

    pop     rbx
    ret



; Args
;   rax: pointer to the tuple
;   rbx: Function
;   rcx: Return type
;   rdx: arg 1
;   r8:  arg 2
; Returns
;   rsi: Array of outputs
tuple~for_each:
    ;; NOTE: Not Impletemented
    ret

    push    r9                  ; Array of outputs
    push    r10                 ; First arg of the called function
    push    r11                 ; Original Tuple Pointer
    push    r12                 ; Counter
    push    r13                 ; Pointer to after the metadata
    push    r14                 ; Size of the tuple
    push    rbx


    lea     r11, [rax]
    mov     r14, [rax + arr#meta#user_size]
    lea     r13, [rax + arr#meta_size]
    xor     r12, r12

    mov     rax, 0
    mov     rbx, rcx

    call    arr#new

    lea     r9, [rsi]

    ;; Move args for the functions
    mov     r10, rdx
    mov     rcx, r8

    xor     rsi, rsi


    .loop:
        mov     rax, r12

        lea     rax, [r11]
        mov     rbx, r12

        call    arr~get

        mov     rax, rsi

        mov     rbx, r10

        call    [rsp]

        mov     rbx, rsi
        lea     rax, [r9]

        call    arr~push

        add     r12, 1
    .loop_check:
        cmp     r12, r14
        jl      .loop

    lea     rsi, [r9]

    pop     rbx
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    ret


%endif                          ; ifdef guard
