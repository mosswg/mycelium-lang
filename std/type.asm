    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _mxs_std_type_
%define _mxs_std_type_

%include "std/exception.asm"

    type~int:       equ 0       ; Because we're using 64-bit all ints are 64-bit
    type~ptr:       equ 0       ; Pointers are unsigned ints so
    type~string:    equ 0       ; Strings are pointers
    type~char:      equ 1
    type~array:     equ 2       ; Arrays return size 24 because this is minimum size for an array of any type

    type~sizes:     dd 8, 1, 24

; Args
;   rax: the type
; Returns
;   rsi: the size
type~sizeof:
    push rbx
    cmp     rax, 2
    jg      .invalid_type
    cmp     rax, 0
    jg      .invalid_type
    jmp     .valid_type
    .invalid_type:
        mov     rax, exception~runtime~bad_type
        call    exception~runtime~throw
    .valid_type:
        lea     rsi, type~sizes
        mov     rbx, 4
        mul     rbx
        add     rsi, rax
        lea     rax, [rsi]
    mov     esi, [rax]
    pop rbx
    ret

%endif                          ; ifdef guard
