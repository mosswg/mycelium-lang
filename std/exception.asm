    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_exception_
%define _Mycelium_std_exception_

%include "std/sys.asm"
%include "std/out.asm"



    ;;  TODO: Change this to an indexable array.
    exception~runtime~bad_exception_number:         equ 0
    exception~runtime~bad_exception_number_str:     db "Invalid Exception Number", 0xA, 0
    exception~runtime~bad_exception_number_len:     equ $-exception~runtime~bad_exception_number_str
    exception~runtime~bad_arg:                      equ 1
    exception~runtime~bad_arg_str:                  db "Invalid Argument Index", 0xA
    exception~runtime~bad_arg_len:                  equ $-exception~runtime~bad_arg_str
    exception~runtime~bad_index:                    equ 2
    exception~runtime~bad_index_str:                db "Invalid Array Index", 0xA
    exception~runtime~bad_index_len:                equ $-exception~runtime~bad_index_str
    exception~runtime~bad_type:                     equ 3
    exception~runtime~bad_type_str:                 db "Unknown Type", 0xA
    exception~runtime~bad_type_len:                 equ $-exception~runtime~bad_type_str
    exception~runtime~bad_allocation:               equ 4
    exception~runtime~bad_allocation_str:           db "Could Not Allocate Memory"
    exception~runtime~bad_allocation_len:           equ $-exception~runtime~bad_allocation_str


; Args
;   rax: the exception number
; Returns
;   void
exception~runtime~throw:
    push    rax

    cmp     rax, exception~runtime~bad_arg
    je      .case_bad_arg
    cmp     rax, exception~runtime~bad_index
    je      .case_bad_index
    cmp     rax, exception~runtime~bad_type
    je      .case_bad_type
    cmp     rax, exception~runtime~bad_allocation
    je      .case_bad_alloc
    jmp     .default

    .case_bad_arg:
        mov     rax, exception~runtime~bad_arg_str
        mov     rbx, exception~runtime~bad_arg_len
        jmp     .switch_end
    .case_bad_index:
        mov     rax, exception~runtime~bad_index_str
        mov     rbx, exception~runtime~bad_index_len
        jmp     .switch_end
    .case_bad_type:
        mov     rax, exception~runtime~bad_type_str
        mov     rbx, exception~runtime~bad_type_len
        jmp     .switch_end
    .case_bad_alloc:
        mov     rax, exception~runtime~bad_allocation_str
        mov     rbx, exception~runtime~bad_allocation_len
        jmp     .switch_end
    .default:
        mov     rax, exception~runtime~bad_exception_number_str
        mov     rbx, exception~runtime~bad_exception_number_len
    .switch_end:
    call    out~put_err
    pop     rax
    call sys~exit
    ret


%endif                          ; ifdef guard
