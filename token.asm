  ;; Author: Moss Gallagher
  ;; Date: 21-Oct-21

%ifndef     _mycelium_token_
%define     _mycelium_token_

%include "std/sys.asm"
%include "std/str.asm"
%include "std/tuple.asm"

section .data
    token#binary#math#plus#code:        equ 0x0
    token#binary#math#plus#cstr:        db "+", 0

    token#binary#math#minus#code:       equ 0x1
    token#binary#math#minus#cstr:       db "-", 0

    token#binary#math#mul#code:         equ 0x2
    token#binary#math#mul#cstr:         db "*", 0

    token#binary#math#div#code:         equ 0x3
    token#binary#math#div#cstr:         db "/", 0

    token#binary#math#mod#code:         equ 0x4
    token#binary#math#mod#cstr:         db "%", 0

    token#binary#math#assign#code:      equ 0x5
    token#binary#math#assing#cstr:      db "=", 0


    token#binary#logic#eq#code:         equ 0x0
    token#binary#logic#eq#cstr:         db "==", 0

    token#binary#logic#neq#code:        equ 0x1
    token#binary#logic#neq#cstr:        db "!=", 0

    token#binary#logic#less#code:       equ 0x2
    token#binary#logic#less#cstr:       db "<", 0

    token#binary#logic#less_eq#code:    equ 0x3
    token#binary#logic#less_eq#cstr:    db "<=", 0

    token#binary#logic#greater#code:    equ 0x4
    token#binary#logic#greater#cstr:    db ">", 0

    token#binary#logic#greater_eq#code: equ 0x5
    token#binary#logic#greater_eq#cstr: db ">=", 0

    token#binary#logic#and#code:        equ 0x6
    token#binary#logic#and#cstr:        db "&&", 0

    token#binary#logic#or#code:         equ 0x7
    token#binary#logic#or#cstr:         db "||", 0


    token#unary#inc#code:               equ 0x0
    token#unary#inc#cstr:               db "++", 0

    token#unary#dec#code:               equ 0x1
    token#unary#dec#cstr:               db "--", 0

    token#unary#not#code:               equ 0x2
    token#unary#not#cstr:               db "!", 0


    token#bitwise#shl#code:             equ 0x0
    token#bitwise#shl#cstr:             db "<<", 0

    token#bitwise#shr#code:             equ 0x1
    token#bitwise#shr#cstr:             db ">>", 0

    token#bitwise#and#code:             equ 0x2
    token#bitwise#and#cstr:             db "&", 0

    token#bitwise#or#code:              equ 0x3
    token#bitwise#or#cstr:              db "|", 0

    token#bitwise#not#code:             equ 0x4
    token#bitwise#not#cstr:             db "~", 0


    token#whitespace#space#code:        equ 0x0
    token#whitespace#space#cstr:        db " ", 0

    token#whitespace#tab#code:          equ 0x1
    token#whitespace#tab#cstr:          db "	", 0


    token#grouping#open#para#code:      equ 0x0
    token#grouping#open#para#cstr:      db "(", 0

    token#grouping#close#para#code:     equ 0x1
    token#grouping#close#para#cstr:     db ")", 0

    token#grouping#open#curly#code:     equ 0x2
    token#grouping#open#curly#cstr:     db "{", 0

    token#grouping#close#curly#code:    equ 0x3
    token#grouping#close#curly#cstr:    db "}", 0

    token#grouping#open#bracket#code:   equ 0x4
    token#grouping#open#bracket#cstr:   db "[", 0

    token#grouping#close#bracket#code:  equ 0x5
    token#grouping#close#bracket#cstr:  db "]", 0


    token#keyword#func#code:            equ 0x0
    token#keyword#func#cstr:            db "fn", 0

    token#keyword#oper#code:            equ 0x1
    token#keyword#oper#cstr:            db "op", 0

    token#keyword#cond#code:            equ 0x2
    token#keyword#cond#cstr:            db "cn", 0


    token#mov:                          db "mov	", 0
    token#add:                          db "add	", 0
    token#sub:                          db "sub	", 0
    token#call:                         db "call	", 0
    token#rax:                          db "rax", 0
    token#rbx:                          db "rbx", 0
    token#rcx:                          db "rcx", 0
    token#rdx:                          db "rdx", 0
    token#rsi:                          db "rsi", 0
    token#comma:                        db ", ", 0

    token#type#number:                  equ 0x1
    token#type#binary_op:               equ 0x2
    token#type#logic_op:                equ 0x3
    token#type#bitwise_op:              equ 0x4
    token#type#unary_op:                equ 0x5
    token#type#whitespace:              equ 0x6
    token#type#grouping:                equ 0x7
    token#type#word:                    equ 0x8
    token#type#func:                    equ 0x9
    token#type#keyword:                 equ 0xa

    token#type#new_line:                equ 0x0




section .bss

    token#binary#math#plus#str:         resq 1

    token#binary#math#minus#str:        resq 1

    token#binary#math#mul#str:          resq 1

    token#binary#math#div#str:          resq 1

    token#binary#math#mod#str:          resq 1

    token#ops#binary#math:              resq 1


    token#binary#logic#eq#str:          resq 1

    token#binary#logic#neq#str:         resq 1

    token#binary#logic#shl#str:         resq 1

    token#binary#logic#shr#str:         resq 1

    token#binary#logic#less#str:        resq 1

    token#binary#logic#less_eq#str:     resq 1

    token#binary#logic#greater#str:     resq 1

    token#binary#logic#greater_eq#str:  resq 1

    token#binary#logic#and#str:         resq 1

    token#binary#logic#or#str:          resq 1

    token#binary#bitwise#and#str:       resq 1

    token#binary#bitwise#or#str:        resq 1

    token#ops#binary#logic:             resq 1


    token#unary#inc#str:                resq 1

    token#unary#dec#str:                resq 1

    token#unary#not#str:                resq 1

    token#ops#unary:                    resq 1


    token#bitwise#shl#str:              resq 1

    token#bitwise#shr#str:              resq 1

    token#bitwise#and#str:              resq 1

    token#bitwise#or#str:               resq 1

    token#bitwise#not#str:              resq 1

    token#ops#bitwise:                  resq 1


    token#whitespace#space#str:         resq 1

    token#whitespace#tab#str:           resq 1

    token#ops#whitespace:               resq 1


    token#grouping#open#para#str:       resq 1

    token#grouping#close#para#str:      resq 1

    token#grouping#open#curly#str:      resq 1

    token#grouping#close#curly#str:     resq 1

    token#grouping#open#bracket#str:    resq 1

    token#grouping#close#bracket#str:   resq 1

    token#ops#grouping:                 resq 1


    token#keyword#func#str:             resq 1

    token#keyword#oper#str:             resq 1

    token#keyword#cond#str:             resq 1

    token#ops#keyword:                  resq 1


section .data
    token#ops:                          dq 0, token#ops#binary#math, token#ops#binary#logic, token#ops#bitwise, token#ops#unary, token#ops#whitespace, token#ops#grouping, 0, 0, token#ops#keyword
    token#num_types:                    equ token#type#keyword+1

section .text

; Args
;   rax: cstr
;   rbx: str
;   rcx: arr
; Returns
;   void
token#make:
    push    rax
    push    rbx

    call    str#new_cs

    pop     rbx
    pop     rax

    mov     [rbx], rsi

    mov     rbx, rsi
    mov     rax, rcx
    call    arr~push

    ret


; Args
;   void
; Returns
;   void
token#binary#math#generate:
    mov     rax, 0
    mov     rbx, type#string
    call    arr#new

    mov     [token#ops#binary#math], rsi

    mov     rcx, rsi


    mov     rax, token#binary#math#plus#cstr
    mov     rbx, token#binary#math#plus#str

    call    token#make


    mov     rax, token#binary#math#minus#cstr
    mov     rbx, token#binary#math#minus#str

    call    token#make


    mov     rax, token#binary#math#mul#cstr
    mov     rbx, token#binary#math#mul#str

    call    token#make


    mov     rax, token#binary#math#div#cstr
    mov     rbx, token#binary#math#div#str

    call    token#make


    mov     rax, token#binary#math#mod#cstr
    mov     rbx, token#binary#math#mod#str

    call    token#make


    %ifdef _mycelium_debug_
    mov     rax, [token#ops#binary#math]
    call    arr~println
    %endif

    ret

; Args
;   void
; Returns
;   void
token#binary#logic#generate:
    mov     rax, 0
    mov     rbx, type#string
    call    arr#new

    mov     [token#ops#binary#logic], rsi
    mov     rcx, rsi



    mov     rax, token#binary#logic#eq#cstr
    mov     rbx, token#binary#logic#eq#str

    call    token#make


    mov     rax, token#binary#logic#neq#cstr
    mov     rbx, token#binary#logic#neq#str

    call    token#make


    mov     rax, token#binary#logic#less#cstr
    mov     rbx, token#binary#logic#less#str

    call    token#make


    mov     rax, token#binary#logic#less_eq#cstr
    mov     rbx, token#binary#logic#less_eq#str

    call    token#make


    mov     rax, token#binary#logic#greater#cstr
    mov     rbx, token#binary#logic#greater#str

    call    token#make


    mov     rax, token#binary#logic#greater_eq#cstr
    mov     rbx, token#binary#logic#greater_eq#str

    call    token#make


    mov     rax, token#binary#logic#and#cstr
    mov     rbx, token#binary#logic#and#str

    call    token#make


    mov     rax, token#binary#logic#or#cstr
    mov     rbx, token#binary#logic#or#str

    call    token#make


    %ifdef _mycelium_debug_
    mov     rax, [token#ops#binary#logic]
    call    arr~println
    %endif

    ret


; Args
;   void
; Returns
;   void
token#unary#generate:
    mov     rax, 0
    mov     rbx, type#string
    call    arr#new

    mov     [token#ops#unary], rsi
    mov     rcx, rsi


    mov     rax, token#unary#inc#cstr
    mov     rbx, token#unary#inc#str

    call    token#make


    mov     rax, token#unary#dec#cstr
    mov     rbx, token#unary#dec#str

    call    token#make


    mov     rax, token#unary#not#cstr
    mov     rbx, token#unary#not#str

    call    token#make


    %ifdef _mycelium_debug_
    mov     rax, [token#ops#unary]
    call    arr~println
    %endif

    ret


; Args
;   void
; Returns
;   void
token#keyword#generate:
    mov     rax, 0
    mov     rbx, type#string
    call    arr#new

    mov     [token#ops#keyword], rsi
    mov     rcx, rsi


    mov     rax, token#keyword#func#cstr
    mov     rbx, token#keyword#func#str

    call    token#make


    mov     rax, token#keyword#oper#cstr
    mov     rbx, token#keyword#oper#str

    call    token#make


    mov     rax, token#keyword#cond#cstr
    mov     rbx, token#keyword#cond#str

    call    token#make

    %ifdef _mycelium_debug_
    mov     rax, [token#ops#keyword]
    call    arr~println
    %endif

    ret

; Args
;   void
; Returns
;   void
token#bitwise#generate:
    mov     rax, 0
    mov     rbx, type#string
    call    arr#new

    mov     [token#ops#bitwise], rsi
    mov     rcx, rsi


    mov     rax, token#bitwise#shl#cstr
    mov     rbx, token#bitwise#shl#str

    call    token#make


    mov     rax, token#bitwise#shr#cstr
    mov     rbx, token#bitwise#shr#str

    call    token#make


    mov     rax, token#bitwise#and#cstr
    mov     rbx, token#bitwise#and#str

    call    token#make


    mov     rax, token#bitwise#or#cstr
    mov     rbx, token#bitwise#or#str

    call    token#make


    mov     rax, token#bitwise#not#cstr
    mov     rbx, token#bitwise#not#str

    call    token#make


    %ifdef _mycelium_debug_
    mov     rax, [token#ops#bitwise]
    call    arr~println
    %endif

    ret

; Args
;   void
; Returns
;   void
token#whitespace#generate:
    mov     rax, 0
    mov     rbx, type#string
    call    arr#new

    mov     [token#ops#whitespace], rsi
    mov     rcx, rsi


    mov     rax, token#whitespace#space#cstr
    mov     rbx, token#whitespace#space#str

    call    token#make


    mov     rax, token#whitespace#tab#cstr
    mov     rbx, token#whitespace#tab#str

    call    token#make


    %ifdef _mycelium_debug_
    mov     rax, [token#ops#whitespace]
    call    arr~println
    %endif

    ret


; Args
;   void
; Returns
;   void
token#grouping#generate:
    mov     rax, 0
    mov     rbx, type#string
    call    arr#new

    mov     [token#ops#grouping], rsi
    mov     rcx, rsi


    mov     rax, token#grouping#open#para#cstr
    mov     rbx, token#grouping#open#para#str

    call    token#make


    mov     rax, token#grouping#close#para#cstr
    mov     rbx, token#grouping#close#para#str

    call    token#make


    mov     rax, token#grouping#open#curly#cstr
    mov     rbx, token#grouping#open#curly#str

    call    token#make


    mov     rax, token#grouping#close#curly#cstr
    mov     rbx, token#grouping#close#curly#str

    call    token#make


    mov     rax, token#grouping#open#bracket#cstr
    mov     rbx, token#grouping#open#bracket#str

    call    token#make


    mov     rax, token#grouping#close#bracket#cstr
    mov     rbx, token#grouping#close#bracket#str

    call    token#make


    %ifdef _mycelium_debug_
    mov     rax, [token#ops#grouping]
    call    arr~println
    %endif

    ret

    ret



; Args
;   void
; Returns
;   void
token#generate:
    call    token#binary#math#generate

    call    token#binary#logic#generate

    call    token#unary#generate

    call    token#bitwise#generate

    call    token#whitespace#generate

    call    token#grouping#generate

    call    token#keyword#generate

    ret

; Args
;   void
; Returns
;   void
token#del:
    push    r8                  ; counter
    push    r9                  ; ops array
    push    r10                 ; array size

    lea     r9, [token#ops]
    mov     r10, token#num_types
    xor     r8, r8

    jmp     .loop_check
    .loop:
        mov     rax, r9
        mov     rbx, r8
        shl     rbx, 3           ; rbx * 8

        mov     rax, [rax + rbx]

        cmp     rax, 0
        je      .no_del
        call    arr~del

        .no_del:
        add     r8, 1
    .loop_check:
        cmp     r8, r10
        jl      .loop


    pop     r10
    pop     r9
    pop     r8
    ret

; Args
;   rax: token
; Returns
;   rsi: type
token.get_data_type:
    push    r8                  ; token type
    push    r9                  ; token

    mov     r9, rax

    mov     rbx, 0
    call    tuple~get             ; rsi = token[0] a.k.a rsi = token.type (token type not data type)

    mov     r8, rsi

    ;; -- Switch --
    cmp     r8, token#type#number
    je      .case_number

    cmp     r8, token#type#word
    je      .case_word

    cmp     r8, token#type#binary_op
    je      .case_op

    cmp     r8, token#type#logic_op
    je      .case_op

    cmp     r8, token#type#bitwise_op
    je      .case_op

    cmp     r8, token#type#unary_op
    je      .case_op

    jmp     .default


    .case_string:
        mov     rsi, type#string
        jmp     .switch_end

    .case_number:
        mov     rsi, type#int
        jmp     .switch_end

    .case_word:
        mov     rax, exception~compiletime~not_implemented
        call    exception~compiletime~throw

    .case_op:
        mov     rsi, type#op
        jmp     .switch_end

    .default:
        mov     rsi, -1

    .switch_end:

    pop     r9
    pop     r8

    ret


; Args
;   rax: token str
;   rbx: token type
; Returns
;   rsi: token code
token#get_code:
    push    r8                  ; counter
    push    r9                  ; token
    push    r10                 ; num tokens
    push    r11                 ; token array

    xor     r8, r8

    mov     r9, rax

    mov     rax, rbx
    call    token#get_array_from_type

    mov     r11, rsi

    cmp     rsi, -1
    je      .no_code

    .loop:
        lea     rax, [r11]
        mov     rbx, r8
        call    arr~get

        mov     rax, rsi
        mov     rbx, 1

        call    tuple~get

        mov     rax, rsi
        mov     rbx, r9
        call    str~eq
        je      .found
    .loop_check:
        cmp     r8, r10
        jl      .loop

    .not_found:
    mov     rax, exception~compiletime~bad_token
    mov     rbx, r9
    call    exception~runtime~throw

    .found:
        mov     rsi, r8
    jmp     .return
    .no_code:
        mov     rsi, -1
    .return:
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret


; Args
;   rax: type
; Return
;   rsi: array
token#get_array_from_type:
    mov     rsi, -1
    cmp     rax, token#type#whitespace
    jg      .return

    shl     rax, 3             ; Multiply by 8 (pointer size)
    mov     rbx, [token#ops]
    mov     rsi, [rbx + rax]
    .return:
    ret

; Args
;   rax: token str
; Returns
;   rsi: token code
;;; FIXME: Not Implemented
token#get_var:
    mov     rsi, -1
    ret
    push    r8                  ; counter
    push    r9                  ; token
    push    r10                 ; num tokens
    push    r11                 ; token array

    xor     r8, r8

    mov     r9, rax
    mov     r11, [token#ops#binary#math]

    mov     rax, r9
    call    str~is_int
    jnz     .return              ; ignore nums their parsed in other places


    .loop:
        lea     rax, [r11]
        mov     rbx, r8
        call    arr~get

        mov     rax, rsi
        mov     rbx, 1

        call    tuple~get

        mov     rax, rsi
        mov     rbx, r9
        call    arr~compare
        je      .found
    .loop_check:
        cmp     r8, r10
        jl      .loop

    .not_found:
    mov     rax, exception~compiletime~bad_token
    mov     rbx, r9
    call    exception~runtime~throw

    .found:
        lea     rax, [r11]
        mov     rbx, r8
        call    arr~get

        mov     rax, rsi
        mov     rbx, 0

        call    tuple~get
    .return:
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret

%endif
