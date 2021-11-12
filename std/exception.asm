    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_exception_
%define _Mycelium_std_exception_

%include "std/sys.asm"
%include "std/out.asm"
%include "std/str.asm"


    ;;  TODO: Change this to an indexable array or list.
    exception~runtime~bad_exception_number:         equ 0
    exception~runtime~bad_exception_number_str:     db "Invalid Exception Number", 0xa
    exception~runtime~bad_exception_number_len:     equ $-exception~runtime~bad_exception_number_str

    exception~runtime~bad_arg:                      equ 1
    exception~runtime~bad_arg_str:                  db "Invalid Argument Index", 0xa
    exception~runtime~bad_arg_len:                  equ $-exception~runtime~bad_arg_str

    exception~runtime~bad_index:                    equ 2
    exception~runtime~bad_index_str:                db "Invalid Index", 0xa
    exception~runtime~bad_index_len:                equ $-exception~runtime~bad_index_str

    exception~runtime~bad_type:                     equ 3
    exception~runtime~bad_type_str:                 db "Unknown Type", 0xa
    exception~runtime~bad_type_len:                 equ $-exception~runtime~bad_type_str

    exception~runtime~bad_allocation:               equ 4
    exception~runtime~bad_allocation_str:           db "Could Not Allocate Memory", 0xa
    exception~runtime~bad_allocation_len:           equ $-exception~runtime~bad_allocation_str

    exception~runtime~not_implemented:              equ 6
    exception~runtime~not_implemented_str:          db "Not Implemented: ", 0xa
    exception~runtime~not_implemented_len:          equ $-exception~runtime~not_implemented_str

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
    cmp     rax, exception~runtime~not_implemented
    je      .case_not_implemented
    jmp     .default

    .case_bad_arg:
        mov     rax, exception~runtime~bad_arg_str
        mov     rbx, exception~runtime~bad_arg_len
        call    out~put_err
        jmp     .switch_end
    .case_bad_index:
        mov     rax, exception~runtime~bad_index_str
        mov     rbx, exception~runtime~bad_index_len
        call    out~put_err
        jmp     .switch_end
    .case_bad_type:
        mov     rax, exception~runtime~bad_type_str
        mov     rbx, exception~runtime~bad_type_len
        call    out~put_err
        jmp     .switch_end
    .case_bad_alloc:
        mov     rax, exception~runtime~bad_allocation_str
        mov     rbx, exception~runtime~bad_allocation_len
        call    out~put_err
        jmp     .switch_end
    .case_not_implemented:
        mov     rax, exception~runtime~not_implemented_str
        mov     rbx, exception~runtime~not_implemented_len
        call    out~put_err
        mov     rax, rbx
        call    cstr~print
        jmp     .switch_end
    .default:
        mov     rax, exception~runtime~bad_exception_number_str
        mov     rbx, exception~runtime~bad_exception_number_len
        call    out~put_err
    .switch_end:
    pop     rax
    neg     rax
    call    sys~exit
    ret


    exception~compiletime~pre:                          db "Compiler Error: ", 0
    exception~compiletime~pre_len:                      equ $-exception~compiletime~pre

    exception~compiletime~bad_exception_number:         equ 0
    exception~compiletime~bad_exception_number_str:     db "Invalid Exception Number", 0xa
    exception~compiletime~bad_exception_number_len:     equ $-exception~compiletime~bad_exception_number_str

    exception~compiletime~bad_token:                    equ 0x1
    exception~compiletime~bad_token_str:                db "Unknown Token ", 0
    exception~compiletime~bad_token_len:                equ $-exception~compiletime~bad_token_str

    exception~compiletime~bad_op_arg:                   equ 0x2
    exception~compiletime~bad_op_arg_str:               db " Operator Expected Value Argument but Found ", 0
    exception~compiletime~bad_op_arg_len:               equ $-exception~compiletime~bad_op_arg_str

    exception~compiletime~not_implemented:              equ 0x3
    exception~compiletime~not_implemented_str:          db "Not Implemented: ", 0xa
    exception~compiletime~not_implemented_len:          equ $-exception~compiletime~not_implemented_str



; Args
;   rax: the exception number
; Returns
;   void
exception~compiletime~throw:
    ;; Switch
    mov     r9, rax
    mov     r10, rbx
    mov     r11, rcx

    mov     rax, exception~compiletime~pre
    mov     rbx, exception~compiletime~pre_len
    call    out~put_err


    cmp     r9, exception~compiletime~bad_token
    je      .case_bad_token
    cmp     r9, exception~compiletime~bad_op_arg
    je      .case_bad_op_arg
    cmp     r9, exception~compiletime~not_implemented
    je      .case_not_implemented
    jmp     .default

    .case_bad_token:
        mov     rax, exception~compiletime~bad_token_str
        mov     rbx, exception~compiletime~bad_token_len
        call    out~put_err
        mov     rax, r10
        call    cstr~print
        jmp     .switch_end
    .case_bad_op_arg:
        mov     rax, r10
        call    cstr~print
        mov     rax, exception~compiletime~bad_op_arg_str
        mov     rbx, exception~compiletime~bad_op_arg_len
        call    out~put_err
        mov     rax, r11
        call    cstr~print
        jmp     .switch_end
    .case_not_implemented:
        mov     rax, exception~compiletime~not_implemented_str
        mov     rbx, exception~compiletime~not_implemented_len
        call    out~put_err
        mov     rax, r10
        call    cstr~print
        jmp     .switch_end
    .default:
        mov     rax, exception~compiletime~bad_exception_number_str
        mov     rbx, exception~compiletime~bad_exception_number_len
        call    out~put_err
    .switch_end:
    mov     rax, 0xa
    call    out~putc
    pop     rax
    neg     rax
    call    sys~exit
    ret





%endif                          ; ifdef guard
