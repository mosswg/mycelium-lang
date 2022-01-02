  ;; Author: Moss Gallagher
  ;; Date: 21-Oct-21

%include "std/sys.asm"
%include "std/str.asm"
%include "std/tuple.asm"
%include "token.asm"

section .data






section .bss

    op.ops: resq 1


section .text

; Args
;   void
; Returns
;   void
op.static_init:
    push    r9

    mov     rax, 1
    mov     rbx, type#tuple

    call    arr#new

    mov     op.ops, rsi

    mov     r9, rsi



    pop     r9

    ret



; Args
;   rax: function name
; Returns
;   rsi: op
op#new:
    push    r9                  ; op
    push    r10                 ; function

    mov     r10, rax

    mov     rax, list#type#basic
    mov     rbx, 1

    call    list#new

    mov     r9, rsi


    mov     rax, r9
    mov     rbx, type#string
    mov     rcx, r10

    mov     rsi, r9

    pop     r10
    pop     r9
    ret


; Args
;   rax: string
; Returns
;   rsi: op
op.create:


    ret



op.operator_pre:       db "O_", 0
; Args
;   rax: array of tokens
; Returns
;   rsi: function name
op.get_function_name_from_operator:
    push    rbp
    push    r8                  ; loop counter
    push    r10                 ; function name
    push    r11                 ; number of tokens
    push    r12                 ; array of tokens
    push    r13                 ; char loop counter
    push    r14                 ; char loop string
    mov     rbp, rsp
    ;;      rbp - 8               temp value
    ;;      rbp - 16              temp string

    mov     r11, [rax + arr#meta#user_size]
    mov     r12, rax

    call    str#new
    mov     r10, rsi

    xor     r8, r8
    jmp     .loop_check
    .loop:
        mov     rax, r12
        mov     rbx, r8

        call    arr~get

        push    rsi
        mov     rax, rsi

        call    token.get_data_type

        cmp     rsi, -1
        je      .loop_end

        cmp     rsi, type#op
        jne     .non_op

        call    str#new

        push    rsi

        pop     rax
        mov     rbx, 1

        call    tuple~get

        mov     r14, rsi

        xor     r13, r13

        jmp     .char_loop_check
        .char_loop:
            mov     rax, r14
            mov     rbx, r13

            call    arr~get

            mov     rax, rsi

            call    int~to_string_rhex

            push    rsi      ; store the string in a temporary place to later be deleted

            mov     rax, [rbp + 8]
            mov     rbx, rsi

            call    arr~concat          ; add the string hex value of the operator to the temp string

            pop     rax

            call    arr~del             ; delete the hex string

            add     r13, 1
        .char_loop_check:
            cmp     r13, [r14 + arr#meta#user_size]

        mov     rax, r10
        pop     rbx
        pop     rbx

        call    arr~concat

        jmp     .loop_end

        .non_op:
            mov     rax, rsi

            call    type~get_name

            mov     rax, rsi

            call    str#new_cs

            mov     [rbp - 8], rsi

            mov     rbx, rsi
            mov     rax, r10

            call    arr~concat

            mov     rax, [rbp - 8]

            call    arr~del

        .loop_end:
        add     r8, 1
    .loop_check:
        cmp     r8, r11
        jl      .loop

    mov     rsi, r10

    pop     r12
    pop     r11
    pop     r10
    pop     r8
    pop     rbp
    ret


op.function_args:       db "A_", 0
op.function_returns:    db "R_", 0
op.sepatarator:         db "_", 0

; Args
;   rax: token
; Returns
;   rsi: string repersentation of the function
op#get_function:
    push    r9                  ; token
    push    rdx                 ; tmp value

    mov     r9, rax

    mov     rbx, 0
    call    tuple~get           ; eq. rsi = token.type


	;; -- Switch --
    cmp     rsi, token#type#func
    je      .case_func
    cmp     rsi, token#type#binary_op
    je      .case_binary_op
    jmp     .default

    .case_func:
        call    str#new
        mov     rdx, rsi

        mov     rax, r9
        mov     rbx, 1
        call    tuple~get       ; eq. rsi = token.function

        mov     rax, rdi
        mov     rbx, rsi
        call    arr~concat

        mov     rsi, rax
        jmp     .switch_end

    .case_binary_op:
        mov     rax, r9
        mov     rbx, 1
        call    tuple~get       ; eq. rsi = token.operator

        mov     rax, rsi
        call    op.get_function_name_from_operator ; eq. rsi = function_from_operator(token.operator)

        jmp     .switch_end

    .default:
        mov     rsi, -1
    .switch_end:

    pop     rdx
    pop     r9
    ret                         ; eq. return rsi

; Args
;   rax: op
;   rbx: list of operands
; Returns
;   rsi: op
op#set_args:
    push    r8                  ; loop counter
    push    r9                  ; operand args
    push    r10                 ; number of operands (length of the operand array)
    push    r11                 ; op

    mov     r9, rbx
    mov     r11, rax

    xor     r8, r8
    mov     r10, [r9 + arr#meta#user_size]

    jmp     .loop_check
    .loop:
        mov     rax, r9
        mov     rbx, r8

        call    arr~get


        mov     rax, r11
        mov     rbx, r8
        mov     rcx, rsi

        call    tuple~set

        add     r8, 1
    .loop_check:
        cmp     r8, r10
        jl      .loop


    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret


; Args
;   rax: token
; Returns
;   rsi: op code
op#new_from_token:
    push    r8                  ; token cat
    push    r9                  ; token index in category
    push    r10                 ; token

    mov     r10, rax

    call    op#get_function

    cmp     rsi, -1
    je      .return             ; Return a -1 if is operand not an operator


    mov     r9, rsi

    shl     r8, 8               ; eq. token.category * 256
    add     r8, r9

    mov     rsi, r8

    .return:
    pop     r10
    pop     r9
    pop     r8
    ret


; Args
;   rax: token array
; Returns
;   rsi: op array
op#create_array_from_tokens:
    push    r8                  ; loop counter
    push    r9                  ; token array
    push    r10                 ; out array
    push    r11                 ; token array length

    mov     r9, rax
    mov     r11, [r9 + arr#meta#user_size]
    xor     r8, r8

    mov     rax, 0
    mov     rbx, type#tuple
    call    arr#new

    mov     r10, rsi

    jmp     .loop_check
    .loop:
        mov     rax, r9
        mov     rbx, r8
        call    arr~get


        mov     rax, rsi

        call    op#new_from_token

        cmp     rsi, -1
        je      .loop_end

        mov     rax, rsi
        mov     rbx, r8
        mov     rcx, r9

        call    op#create_op

        add     r8, rdi


        mov     rbx, rsi
        mov     rax, r10

        call    arr~push

        jmp     .loop_check     ; Since we've already added to r8 we just want to skip to the loop check

    .loop_end:
        add     r8, 1
    .loop_check:
        cmp     r8, r11
        jl      .loop

    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret


; Args
;   rax: index in token array
;   rbx: token array
; Returns
;   rsi: op
op#create_op:
    push    r8                  ; Create op func
    push    r9                  ; op code
    push    r10                 ; out op

    mov     rax, r9
    call    op#new

    mov     r10, rsi

    mov     rax, r9

    call    op#get_args

    mov     rax, r10
    mov     rbx, rsi

    call    r8

    pop     r10
    pop     r9
    pop     r8
    ret


; Args
;   rax: op code
;   rbx: index in token array
;   rcx: token array
; Returns
;   rsi: array of args
op#get_args:
    push    rbp
    push    r8                  ; loop counter
    push    r9                  ; op code
    push    r11                 ; arg positions
    push    r12                 ; token array
    push    r13                 ; token index
    push    r14                 ; arg array
    mov     rbp, rsp
;;; rbp - 8                       current arg

    mov     r9, rax
    mov     r12, rcx

    mov     rax, 0
    mov     rbx, type#tuple
    call    arr#new

    mov     r14, rsi

    mov     rax, r9
    shl     rax, 3              ; mul by 8

    jmp     .loop_check
    .loop:
        mov     rax, 2
        call    tuple#new

        mov     [rbp-8], rsi

        mov     rax, r12
        mov     rbx, r13
        mov     rcx, r8
        call    op#get_arg

        mov     rdx, rdi
        mov     rcx, rsi
        mov     rbx, 1
        mov     rax, [rbp-8]

        call    tuple~set       ; Set the second element of the tuple to be the value of the arg

        mov     rcx, rdx
        mov     rbx, 0
        mov     rax, [rbp-8]

        call    tuple~set       ; Set the first element of the tuple to be the type of the arg

        mov     rax, r14
        mov     rbx, [rbp-8]

        call    arr~push

        add     r8, 1
    .loop_check:
        cmp     r8, r10
        jl      .loop


    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r9
    pop     rbp
    ret


; Args
;   rax: token array
;   rbx: token index
; Returns
;   rsi: pre_arg
op#get_pre_arg:

    push    r8                  ; loop counter
    push    r9                  ; token array size
    push    r10                 ; token array
    push    r11                 ; token index
    push    r12                 ; arg pos
    push    r13                 ; current token

    mov     r12, rcx
    mov     r11, rbx
    mov     r10, rax
    mov     r9, [rax + arr#meta#user_size]

    xor     r8, r8
    jmp     .loop_check
    .loop:
        mov     rax, r10
        mov     rbx, r8
        call    arr~get

        mov     r13, rsi

        mov     rax, rsi
        mov     rbx, 0
        call    tuple~get

        cmp     rsi, token#type#whitespace
        je      .loop_end

        cmp     rsi, token#type#grouping
        jne     .non_grouped

        mov     rax, r10
        mov     rbx, r11
        mov     rcx, -1
        call    op#eval_grouped

        .non_grouped:
        mov     rax, r13
        mov     rbx, 0
        call    tuple~get
        mov     r8, rsi

        mov     rax, r13
        mov     rbx, 0
        call    tuple~get

        mov     rdi, r8
        jmp     .return

    .loop_end:
        sub     r8, 1
    .loop_check:
        cmp     r8, r9
        jl      .loop

    .return:
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret



    ret


op#arg_none_cstr:       db "None", 0


; Args
;   rax: token array
;   rbx: token index
;   rcx: arg index
; Returns
;   rsi: arg value
;   rdi: arg type
op#get_arg:
    push    rbp
    push    r8                  ; loop counter
    push    r9                  ; token array size
    push    r10                 ; token array
    push    r11                 ; token index
    push    r12                 ; arg pos
    push    r13                 ; arg index
    push    r14                 ; arg count
    mov     rbp, rsp
;;; rbp - 8                       current token

    mov     r12, rcx
    mov     r11, rbx
    mov     r10, rax
    mov     r9, [rax + arr#meta#user_size]
    xor     r14, r14

    xor     r8, r8
    jmp     .loop_check
    .loop:
        mov     rax, r10
        mov     rbx, r8
        call    arr~get

        mov     [rbp-8], rsi

        mov     rax, rsi
        mov     rbx, 0
        call    tuple~get

        cmp     rsi, token#type#whitespace
        je      .loop_end

        cmp     rsi, token#type#grouping
        jne     .non_grouped

        mov     rax, r10
        mov     rbx, r11
        mov     rcx, 1
        call    op#eval_grouped

        jmp     .verify_arg
        .non_grouped:
        mov     rax, [rbp-8]
        mov     rbx, 0
        call    tuple~get
        mov     r8, rsi

        mov     rax, [rbp-8]
        mov     rbx, 0
        call    tuple~get

        mov     rdi, r8
        jmp     .verify_arg

    .loop_end:
        add     r8, 1
    .loop_check:
        cmp     r8, r9
        jl      .loop

    mov     rax, r10
    mov     rbx, r11
    call    arr~get

    mov     rax, rsi
    mov     rbx, 1
    call    tuple~get

    mov     rbx, rsi
    mov     rax, exception~compiletime~bad_op_arg
    mov     rcx, op#arg_none_cstr

    call    exception~compiletime~throw

    .verify_arg:
        cmp     r14, r13
        add     r14, 1
        jne     .loop_check
    .return:
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rbp
    ret

; Args
;   rax: token array
;   rbx: start index
;   rcx: direction
; Returns
;   rsi: op
op#eval_grouped:



    ret
