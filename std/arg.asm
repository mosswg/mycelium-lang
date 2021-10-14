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
    lea     rbx, [r15]

    cmp     rax, [rbx]                          ; Verify that the given index exists
    jl      .valid_arg
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw
    .valid_arg:
    add     rbx, rax
    mov     rsi, [rbx]
    pop     rbx
    ret


%endif                          ; ifdef guard
