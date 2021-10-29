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
    empty_str:          resq 1

section .data
    empty_cstr:          db 0
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

    mov     rax, empty_cstr
    call    str#new_cs

    mov     [empty_str], rsi


    mov     rax, [token#ops#binary#math]
    mov     rbx, [token#ops#binary#logic]

    call    arr#concat          ; Create the split array with the values of binary math and binary logic combined


    mov     [split_array], rsi

    mov     rax, rsi
    mov     rbx, [token#ops#unary]

    call    arr~concat          ; Add the unary strings to the split array


    mov     rax, [split_array]
    mov     rbx, [token#ops#bitwise]

    call    arr~concat          ; Add the bitwise strings to the split array


    mov     rax, [split_array]
    mov     rbx, [token#ops#whitespace]

    call    arr~concat          ; Add the whitespace strings into the split array


    mov     rax, [split_array]
    mov     rbx, [token#ops#grouping]

    call    arr~concat          ; Add the grouping strings to the split array


    mov     rax, [split_array]
    call    arr~println         ; Print the array of split strings


    mov     rax, r10
    mov     rbx, str~split_a_s
    mov     rcx, type#arr
    mov     rdx, [split_array]
    call    arr~for_each        ; split each line on spaces (we should add tabs and other white space later)

    lea     r10, [rsi]

    mov     rax, r10
    call    arr~printn          ; Print the arrays of split lines

    xor     r8, r8              ; Clear the line loop counter

    jmp     .line_loop_check
    .line_loop:
        lea     rax, [r10]
        mov     rbx, r8
        call    arr~get

        lea     r12, [rsi]

        mov     r13, [r12 + arr#meta#user_size]
        xor     r9, r9          ; Clear the word counter for every line
        jmp     .word_loop_check
        .word_loop:
            xor     rsi, rsi

            lea     rax, [r12]
            mov     rbx, r9
            call    arr~get

            mov     rax, [rsi + arr#meta#user_size] ; get the size of the string
            cmp     rax, 0                          ; check if the string is size zero
            je      .word_loop_check

            mov     [rbp-8], rsi


            push    rsi
            push    type#string

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
            je      .case_unary

            mov     rbx, [rbp-8]
            mov     rcx, type#string
            mov     rax, [token#ops#bitwise]
            call    arr~contains
            je      .case_bitwise

            mov     rbx, [rbp-8]
            mov     rcx, type#string
            mov     rax, [token#ops#whitespace]
            call    arr~contains
            je      .case_whitespace

            mov     rbx, [rbp-8]
            mov     rcx, type#string
            mov     rax, [token#ops#grouping]
            call    arr~contains
            je      .case_grouping


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

            .case_unary:
                push    token#type#unary_op
                push    type#int
                jmp     .switch_end

            .case_bitwise:
                push    token#type#bitwise_op
                push    type#int
                jmp     .switch_end

            .case_whitespace:
                push    token#type#whitespace
                push    type#int
                jmp     .switch_end

            .case_grouping:
                push    token#type#grouping
                push    type#int
                jmp     .switch_end

            .case_number:
                push    token#type#number
                push    type#int
                jmp     .switch_end

            .case_line_end:
                push    token#type#new_line
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

        .word_loop_end:
            add     r9, 1
        .word_loop_check:
            cmp     r9, r13
            jl      .word_loop

    mov     rax, [empty_str]
    push    rax
    push    type#string

    push    token#type#new_line
    push    type#int

    mov     rax, 2
    lea     rbx, [rsp]
    call    tuple#new_ca

    add     rsp, 4 * 8

    lea     rax, [r14]
    mov     rbx, rsi
    call    arr~push

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
