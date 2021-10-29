  ;; Author: Moss Gallagher
  ;; Date: 21-Oct-21

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

    token#binary#logic#shl#code:        equ 0x2
    token#binary#logic#shl#cstr:        db "<<", 0

    token#binary#logic#shr#code:        equ 0x3
    token#binary#logic#shr#cstr:        db ">>", 0

    token#binary#logic#less#code:       equ 0x4
    token#binary#logic#less#cstr:       db "<", 0

    token#binary#logic#less_eq#code:    equ 0x5
    token#binary#logic#less_eq#cstr:    db "<=", 0

    token#binary#logic#greater#code:    equ 0x6
    token#binary#logic#greater#cstr:    db ">", 0

    token#binary#logic#greater_eq#code: equ 0x7
    token#binary#logic#greater_eq#cstr: db ">=", 0

    token#binary#logic#and#code:        equ 0x8
    token#binary#logic#and#cstr:        db "&&", 0

    token#binary#logic#or#code:         equ 0x9
    token#binary#logic#or#cstr:         db "||", 0

    token#binary#bitwise#and#code:      equ 0xa
    token#binary#bitwise#and#cstr:      db "&", 0

    token#binary#bitwise#or#code:       equ 0xb
    token#binary#bitwise#or#cstr:       db "|", 0


    token#unary#inc#code:               equ 0x0
    token#unary#inc#cstr:               db "++", 0

    token#unary#dec#code:               equ 0x1
    token#unary#dec#cstr:               db "--", 0

    token#unary#not#code:               equ 0x2
    token#unary#not#cstr:               db "!", 0

    token#mov:                          db "mov	", 0
    token#add:                          db "add	", 0
    token#sub:                          db "sub	", 0
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
    token#type#word:                    equ 0x5



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


    mov     rax, token#binary#math#plus#cstr

    call    str#new_cs

    mov     [token#binary#math#plus#str], rsi

    mov     rbx, rsi
    mov     rax, [token#ops#binary#math]
    call    arr~push


    mov     rax, token#binary#math#minus#cstr

    call    str#new_cs

    mov     [token#binary#math#minus#str], rsi

    mov     rbx, rsi
    mov     rax, [token#ops#binary#math]
    call    arr~push


    mov     rax, token#binary#math#mul#cstr

    call    str#new_cs

    mov     [token#binary#math#mul#str], rsi

    mov     rbx, rsi
    mov     rax, [token#ops#binary#math]
    call    arr~push


    mov     rax, token#binary#math#div#cstr

    call    str#new_cs

    mov     [token#binary#math#div#str], rsi

    mov     rbx, rsi
    mov     rax, [token#ops#binary#math]
    call    arr~push


    mov     rax, token#binary#math#mod#cstr

    call    str#new_cs

    mov     [token#binary#math#mod#str], rsi

    mov     rbx, rsi
    mov     rax, [token#ops#binary#math]
    call    arr~push


    mov     rax, [token#ops#binary#math]
    call    arr~println
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


    mov     rax, token#binary#logic#shl#cstr
    mov     rbx, token#binary#logic#shl#str

    call    token#make


    mov     rax, token#binary#logic#shr#cstr
    mov     rbx, token#binary#logic#shr#str

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


    mov     rax, [token#ops#binary#logic]
    call    arr~println
    ret

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


    mov     rax, [token#ops#unary]
    call    arr~println
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

    ret

; Args
;   rax: token str
; Returns
;   rsi: token code
token#get_code:
    push    r8                  ; counter
    push    r9                  ; token
    push    r10                 ; num tokens
    push    r11                 ; token array

    xor     r8, r8

    mov     r9, rax
    mov     r11, [token#ops#binary#math]

    mov     rax, r9
    call    str~is_int
    jz      .number              ; ignore nums they're parsed in other places


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
        mov     rsi, r8
    jmp     .return
    .number:
        mov     rsi, -1
    .return:
    pop     r11
    pop     r10
    pop     r9
    pop     r8
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
