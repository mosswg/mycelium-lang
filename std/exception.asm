    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _mxs_std_exception_
%define _mxs_std_exception_

%include "std/sys.asm"
%include "std/out.asm"


    exception~runtime~bad_exception_number:         equ 0
    exception~runtime~bad_exception_number_str:     db "Invalid Exception Number"
    exception~runtime~bad_exception_number_len:     equ $-exception~runtime~bad_exception_number_str
    exception~runtime~bad_index:                    equ 1
    exception~runtime~bad_index_str:                db "Invalid Index"
    exception~runtime~bad_index_len:                equ $-exception~runtime~bad_index_str


; Args
;   rax: the exception number
; Returns
;   void
exception~runtime~throw:
    push    rax

    cmp     rax, 1
    je      .case_0

    jmp     .default

    .case_0:
        mov     rax, exception~runtime~bad_index_str
        mov     rbx, exception~runtime~bad_index_len
        call    out~puts
        jmp     .switch_end
    .default:
        mov     rax, exception~runtime~bad_exception_number_str
        mov     rbx, exception~runtime~bad_exception_number_len
        call    out~puts
    .switch_end:
    mov     rax, 0xA
    call    out~putc            ; New line
    pop     rax
    call sys~exit
    ret


%endif                          ; ifdef guard
