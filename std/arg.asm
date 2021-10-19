    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_arg_
%define _Mycelium_std_arg_

%include "std/exception.asm"
%include "std/str.asm"


    usage:              db "Usage: ", 0


; Args
;   rax: the index of the args
; Returns
;   rsi: pointer to the string
arg~get:
    push    rbx

    cmp     rax, [r15]                          ; Verify that the given index exists
    jle     .valid_arg
    mov     rax, exception~runtime~bad_arg
    call    exception~runtime~throw
    .valid_arg:
    mov     rbx, 8
    mul     rbx
    lea     rbx, [r15]
    add     rbx, rax
    mov     rsi, [rbx]
    pop     rbx
    ret

; Args
;   rax:    the amount of required args
;   stack:  names of args
; Returns
;   void
arg~require:
    cmp     rax, [r15]
    jl     .valid_args

    mov     r9, rax             ; We don't need to push because if we're here we're going to exit the program and therefore we don't care.

    mov     rax, usage
    call    str~print

    mov     rax, [r15+8]
    call    str~print

    mov     rbx, 0
    mov     rcx, 8

    .arg_loop:
        mov     rax, ' '
        call    out~putc

        mov     rax, rbx
        mul     rcx
        mov     r10, rsp
        add     r10, 8
        add     r10, rax

        mov     rax, [r10]
        call    str~print

        add     rbx, 1
    .arg_loop_check:
        cmp     rbx, r9
        jl      .arg_loop


    mov     rax, 0xA
    call    out~putc

    mov     rax, 1
    call    sys~exit
    .valid_args:
    ret

; Args
;   void
; Returns
;   rsi: Number of args
arg~length:
    mov     rsi, [r15]
    ret


%endif                          ; ifdef guard
