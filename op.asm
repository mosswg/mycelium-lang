  ;; Author: Moss Gallagher
  ;; Date: 21-Oct-21

%include "std/sys.asm"
%include "std/str.asm"
%include "std/tuple.asm"
%include "token.asm"

section .data
    ;; Ops are a tuple with the values [op code, before op arg, after op arg 1, after op arg 2, ..., after op arg n]


    op#add:             equ 0x0
    op#sub:             equ 0x1
    op#mul:             equ 0x2
    op#div:             equ 0x3
    op#mod:             equ 0x4

    op#has_pre_arg:     dq 1,               1,              1,              1,          1
    op#arg_sizes:       dq 2,               2,              2,              2,          2
    op#create_ops:      dq op#create_add,   0,              0,              0,          0



section .text

; Args
;   rax: op code
; Returns
;   rsi: op
op#new:
    push    r8              ; op code

    shl     rax, 3          ; Multiply be 8

    mov     rax, [op#arg_sizes + rax]

    add     rax, 1

    call    tuple#new

    pop     r8
    ret



; Args
;   rax: op
;   rbx: list of operands
;   rcx: second operand
; Returns
;   rsi: op
op#set_args:
    push    r8                  ; loop counter
    push    r9                  ; operand args
    push    r10                 ; number of operands (length of the operand array)

    mov     r9, rax

    xor     r8, r8
    mov     r10, [r9 + arr#meta#user_size]


    pop     r10
    pop     r9
    pop     r8
    ret


;; token -> op
;; (token.category * 256) + token.index
;; e.g. '-' is 0x1 and '<' is 0x102
;; This should be fine because we shouldn't ever have a category with more than 256 items

; Args
;   rax: token
; Returns
;   rsi: op code
op#get_code_from_token:
    push    r8                  ; token cat
    push    r9                  ; token index in category
    push    r10                 ; token

    mov     r10, rax

    mov     rbx, 0

    call    tuple~get           ; eq. token[0]


    mov     r8, rsi

    mov     rax, r10
    mov     rbx, 1

    call    tuple~get           ; eq. token[1]


    mov     rax, rsi
    mov     rbx, r8

    call    token#get_code

    cmp     rsi, -1
    je      .return             ; Return a -1 if is operand


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

        call    op#get_code_from_token

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
;   rax: op code
;   rbx: index in token array
;   rcx: token array
; Returns
;   rsi: op
op#create_op:
    push    r8                  ; Create op func
    push    r9                  ; op code
    push    r10                 ; out op

    mov     r9, rax

    shl     rax, 3              ; Multiply rax by 8 (size of ptr)
    mov     r8, [op#create_ops + rax]

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
    push    r10                 ; number of args
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

    mov     r10, [op#arg_sizes + rax]

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
    pop     r10
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
