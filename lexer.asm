  ;; Author: Moss Gallagher
  ;; Date: 21-Oct-21

%include "std/sys.asm"
%include "std/str.asm"
%include "std/file.asm"
%include "std/tuple.asm"
%include "std/int.asm"
%include "token.asm"


section .bss
    split_array:        resq 1

section .data
    space_str:          db " ", 0
    tab_str:            db "	", 0

section .text

; Args
;   rax: array of lines
; Returns
;   rsi: array of ops
lexer#lex_lines:
    push    rbp
    mov     rbp, rsp
    push    r8                  ; line counter
    push    r9                  ; word counter
    push    r10                 ; line array
    push    r11                 ; line num
    push    r12                 ; current line words
    push    r13                 ; num words on line
    push    r14                 ; out array
;;; rsp - 8                       temp value


    lea     r10, [rax]
    mov     r11, [rax + arr#meta#user_size]

    mov     rax, 0
    mov     rbx, type#tuple
    call    arr#new

    lea     r14, [rsi]

    mov     rax, [token#ops#binary#math]
    mov     rbx, [token#ops#binary#logic]

    call    arr#concat

    mov     [split_array], rsi

    mov     rax, rsi
    mov     rbx, [token#ops#unary]

    call    arr~concat

    mov     rax, space_str
    call    str#new_cs

    mov     rax, [split_array]
    mov     rbx, rsi

    call    arr~push

    mov     rax, tab_str
    call    str#new_cs

    mov     rax, [split_array]
    mov     rbx, rsi

    call    arr~push


    mov     rax, [split_array]
    call    arr~println


    mov     rax, r10
    mov     rbx, str~split_a_s
    mov     rcx, type#arr
    mov     rdx, [split_array]
    call    arr~for_each        ; split each line on spaces (we should add tabs and other white space later)

    lea     r10, [rsi]

    mov     rax, r10
    call    arr~printn

    jmp     .line_loop_check
    .line_loop:
        lea     rax, [r10]
        mov     rbx, r8
        call    arr~get

        lea     r12, [rsi]

        mov     r13, [r12 + arr#meta#user_size]
        jmp     .word_loop_check
        .word_loop:
            xor     rsi, rsi

            lea     rax, [r12]
            mov     rbx, r9
            call    arr~get

            mov     [rbp-8], rsi

            ;; Switch
            mov     rbx, [rbp-8]
            mov     rcx, type#string

            mov     rax, [token#ops#binary#logic]
            call    arr~contains
            je      .case_binary

            mov     rbx, [rbp-8]
            mov     rcx, type#string
            mov     rax, [token#ops#binary#math]
            call    arr~contains
            je      .case_logical

            mov     rbx, [rbp-8]
            mov     rcx, type#string
            mov     rax, [token#ops#unary]
            call    arr~contains
            je      .case_bitwise

            mov     rax, [rbp-8]
            call    str~is_int
            je      .case_number

            jmp     .default

            .case_binary:
                push    token#type#binary_op
                push    type#int
                jmp     .switch_end

            .case_logical:
                push    token#type#logic_op
                push    type#int
                jmp     .switch_end

            .case_bitwise:
                push    token#type#bitwise_op
                push    type#int
                jmp     .switch_end

            .case_number:
                push    token#type#number
                push    type#int
                jmp     .switch_end

            .default:
                push    token#type#word
                push    type#int

            .switch_end:
                mov     rax, 2
                lea     rbx, [rsp]
                call    tuple#new_ca

                add     rsp, 4 * 8

                lea     rax, [r14]
                mov     rbx, rsi
                call    arr~push


            add     r9, 1
        .word_loop_check:
            cmp     r9, r13
            jl      .word_loop

    add     r8, 1
    .line_loop_check:
        cmp     r8, r11
        jl      .line_loop

    lea     rsi, [r14]

    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rbp
    ret
