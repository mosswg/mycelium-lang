    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_type_
%define _Mycelium_std_type_

%include "std/exception.asm"
%include "std/int.asm"
%include "std/list.asm"
%include "std/str.asm"
%include "std/out.asm"

    type#int:       equ 0       ; Because we're using 64-bit all ints are 64-bit
    type#ptr:       equ 1       ; Pointers are unsigned ints so
    type#string:    equ 2       ; Strings are pointers
    type#list:     equ 3        ; Arrays are pointers
    type#char:      equ 4

    type#sizes:     dd 8, 8, 8, 8, 1

; Args
;   rax: the type
; Returns
;   rsi: the size
type~sizeof:
    push    rax
    push    rbx
    cmp     rax, 4
    jg      .invalid_type
    cmp     rax, 0
    jg      .invalid_type
    jmp     .valid_type
    .invalid_type:
        mov     rax, exception~runtime~bad_type
        call    exception~runtime~throw
    .valid_type:
        lea     rsi, type#sizes
        mov     rbx, 4
        mul     rbx
        add     rsi, rax
        lea     rax, [rsi]
    mov     esi, [rax]
    pop     rbx
    pop     rax
    ret

; Args
;   rax: pointer
;   rbx: type
; Returns
;   rsi: value of type at pointer
type~read_mem:
    push    rcx
    ;; -- switch --
    cmp     rbx, type#int
    je      .case_8byte
    cmp     rbx, type#ptr
    je      .case_8byte
    cmp     rbx, type#string
    je      .case_8byte
    cmp     rbx, type#list
    je      .case_8byte
    cmp     rbx, type#char
    je      .case_1byte


    .case_8byte:
        mov     rsi, [rax]
        jmp     .switch_end     ; break
    .case_1byte:
        mov     rcx, 0x0        ; Clear rcx
        mov     rcx, [rax]       ; Put value into lower 8-bits
        mov     rsi, rcx
        jmp     .switch_end     ;break
    .switch_end:

    pop     rcx

    ret

; Args
;   rax: value
;   rbx: type
; Returns
;   void
type~print:
    ;; -- switch --
    cmp     rbx, type#int
    je      .case_int
    cmp     rbx, type#ptr
    je      .case_int
    cmp     rbx, type#string
    je      .case_string
    cmp     rbx, type#list
    je      .case_list
    cmp     rbx, type#char
    je      .case_char


    .case_int:
        call int~print
        jmp     .switch_end     ; break
    .case_string:
        call str~print
        jmp     .switch_end     ; break
    .case_list:
    call list~print
        jmp     .switch_end     ; break
    .case_char:
        call out~putc
        jmp     .switch_end     ;break
    .switch_end:
    ret


%endif                          ; ifdef guard
