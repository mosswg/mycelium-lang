    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_type_
%define _Mycelium_std_type_

%include "std/exception.asm"
%include "std/int.asm"
%include "std/tuple.asm"
%include "std/list.asm"
%include "std/arr.asm"
%include "std/str.asm"
%include "std/cstr.asm"
%include "std/out.asm"

    type#int:       equ 0       ; Because we're using 64-bit all ints are 64-bit
    type#ptr:       equ 1       ; Pointers are unsigned ints
    type#string:    equ 2       ; Strings are pointers
    type#list:      equ 3       ; Lists are pointers
    type#arr:       equ 4       ; Arrays are pointers
    type#tuple:     equ 5       ; Tuples are pointers
    type#cstring:   equ 6       ; cstrings are pointers
    type#op:        equ 7       ; operators are stored as strings
    type#char:      equ 8


    type#int#name:      db "int", 0
    type#ptr#name:      db "ptr", 0
    type#string#name:   db "string", 0
    type#list#name:     db "list", 0
    type#arr#name:      db "arr", 0
    type#tuple#name:    db "tuple", 0
    type#cstring#name:  db "twine", 0
    type#op#name:       db "operator", 0
    type#char#name:     db "char", 0

    type#names:     dq type#int#name, type#ptr#name, type#string#name, type#list#name, type#arr#name, type#tuple#name, type#cstring#name, type#op#name, type#char#name
    type#sizes:     dd 8, 8, 8, 8, 8, 8, 8, 8, 1

; Args
;   rax: the type
; Returns
;   rsi: the size
type~sizeof:
    push    rax
    cmp     rax, type#char
    jg      .invalid_type
    cmp     rax, 0
    jl      .invalid_type
    jmp     .valid_type
    .invalid_type:
        mov     rax, exception~runtime~bad_type
        call    exception~runtime~throw
    .valid_type:
        lea     rsi, type#sizes
        shl     rax, 2
        lea     rax, [rsi+rax]
    mov     esi, [rax]
    pop     rax
    ret



; Args
;   rax: the type
; Returns
;   rsi: name
type~get_name:
    push    rax
    cmp     rax, type#char
    jg      .invalid_type
    cmp     rax, 0
    jl      .invalid_type
    jmp     .valid_type
    .invalid_type:
        mov     rax, exception~runtime~bad_type
        call    exception~runtime~throw
    .valid_type:
        lea     rsi, type#names
        shl     rax, 3
        lea     rax, [rsi+rax]
    mov     rsi, [rax]
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
    cmp     rbx, type#cstring
    je      .case_8byte
    cmp     rbx, type#list
    je      .case_8byte
    cmp     rbx, type#arr
    je      .case_8byte
    cmp     rbx, type#tuple
    je      .case_8byte
    cmp     rbx, type#op
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
;   rax: type
;   rbx: var
; Returns
;   void
type~del:
    ;; -- switch --
    push    rax
    mov     rax, rbx            ; Swap rax and rbx
    pop     rbx

    cmp     rbx, type#int
    je      .case_native_type

    cmp     rbx, type#ptr
    je      .case_native_type

    cmp     rbx, type#string
    je      .case_string

    cmp     rbx, type#cstring
    je      .case_cstring

    cmp     rbx, type#list
    je      .case_list

    cmp     rbx, type#arr
    je      .case_arr

    cmp     rbx, type#tuple
    je      .case_tuple

    cmp     rbx, type#op
    je      .case_string

    cmp     rbx, type#char
    je      .case_native_type


    .case_native_type:
        jmp     .switch_end     ; break
    .case_string:
        call    arr~del
        jmp     .switch_end     ; break
    .case_cstring:
        call    cstr~del
        jmp     .switch_end     ; break
    .case_list:
        call    list~del
        jmp     .switch_end     ; break
    .case_tuple:
        call    tuple~del
        jmp     .switch_end     ; break
    .case_arr:
        call    arr~del
        jmp     .switch_end     ; break
    .default:
        mov     rax, exception~runtime~bad_type
        call    exception~runtime~throw
    .switch_end:
    ret

; Args
;   rax: type
;   rbx: Compare 1
;   rcx: Compare 2
; Returns
;   void
type~compare:
    push    r9                  ; Perserve temp value
    ;; -- switch --
    cmp     rax, type#int
    je      .case_int

    cmp     rax, type#ptr
    je      .case_int

    cmp     rax, type#string
    je      .case_string

    cmp     rax, type#cstring
    je      .case_cstring

    cmp     rax, type#list
    je      .case_list

    cmp     rax, type#arr
    je      .case_arr

    cmp     rax, type#tuple
    je      .case_tuple

    cmp     rax, type#op
    je      .case_string

    cmp     rax, type#char
    je      .case_char


    .case_int:
        cmp     rbx, rcx
        jmp     .switch_end     ; break
    .case_string:
        mov     rax, rbx
        mov     rbx, rcx
        call    str~eq
        jmp     .switch_end     ; break
    .case_cstring:
        mov     rax, rbx
        mov     rbx, rcx
        call    cstr~compare
        jmp     .switch_end     ; break
    .case_list:
        mov     rax, rbx
        mov     rbx, rcx
        call    list~compare
        jmp     .switch_end     ; break
    .case_tuple:
        jmp     .switch_end     ; break
    .case_arr:
        mov     rax, rbx
        mov     rbx, rcx
        call    arr~compare
        jmp     .switch_end     ; break
    .case_char:
        cmp     bl, cl
        jmp     .switch_end     ;break
    .switch_end:
    pop     r9
    ret



; Args
;   rax: value
;   rbx: type
; Returns
;   void
type~print:
    push    r9                  ; Perserve temp value
    ;; -- switch --
    cmp     rbx, type#int
    je      .case_int

    cmp     rbx, type#ptr
    je      .case_int

    cmp     rbx, type#string
    je      .case_string

    cmp     rbx, type#cstring
    je      .case_cstring

    cmp     rbx, type#list
    je      .case_list

    cmp     rbx, type#tuple
    je      .case_tuple

    cmp     rbx, type#arr
    je      .case_arr

    cmp     rbx, type#op
    je      .case_string

    cmp     rbx, type#char
    je      .case_char


    .case_int:
        call    int~print
        jmp     .switch_end     ; break
    .case_string:
        mov     r9, rax

        mov     rax, 0x22       ; "
        call    out~putc

        mov     rax, r9
        call    str~print

        mov     rax, 0x22       ; "
        call    out~putc

        mov     rax, r9

        jmp     .switch_end     ; break
    .case_cstring:
        mov     r9, rax

        mov     rax, 0x22       ; "
        call    out~putc

        mov     rax, r9
        call    cstr~print

        mov     rax, 0x22       ; "
        call    out~putc

        mov     rax, r9

        jmp     .switch_end     ; break
    .case_list:
        call    list~print
        jmp     .switch_end     ; break
    .case_arr:
        call    arr~print
        jmp     .switch_end     ; break
    .case_tuple:
        call    tuple~print
        jmp     .switch_end     ; break
    .case_char:
        mov     r9, rax

        mov     rax, 0x27       ; '
        call    out~putc

        mov     rax, r9
        call    out~putc

        mov     rax, 0x27       ; '
        call    out~putc

        mov     rax, r9

        jmp     .switch_end     ;break
    .switch_end:
    pop     r9
    ret


%endif                          ; ifdef guard
