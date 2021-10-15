    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _mxs_std_arg_
%define _mxs_std_arg_

%include "std/exception.asm"

; Args
;   rax: the index of the args
; Returns
;   rsi: pointer to the string
arg~get:
    push    rbx

    cmp     rax, [r15]                          ; Verify that the given index exists
    jle     .valid_arg
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw
    .valid_arg:
    mov     rbx, 8
    mul     rbx
    mov     rbx, r15
    add     rbx, rax
    mov     rsi, [rbx]
    pop     rbx
    ret


%endif                          ; ifdef guard
