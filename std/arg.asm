    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_arg_
%define _Mycelium_std_arg_

%include "std/exception.asm"

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
;   void
; Returns
;   rsi: Number of args
arg~argc:
    mov     rsi, [r15]
    ret


%endif                          ; ifdef guard
