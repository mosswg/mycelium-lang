	;; Author: Moss Gallagher
	;; Date:	23-Oct-21

%include "std/sys.asm"
%include "std/str.asm"
%include "std/file.asm"
%include "std/tuple.asm"
%include "std/int.asm"
%include "lexer.asm"

global _start

section .data
    filename:       db "examples/add.myc", 0
    outfile:        db "add.myc.asm", 0


    mycelium#file_section_data:        db '%include "std/sys.asm"', 0xa, 0xa, "global _start", 0xa, 0xa, "section .data", 0xa, 0
    mycelium#file_section_text:        db "section .text", 0xa, 0xa, "_start:", 0xa, "	mov   r15, rsp", 0xa, "	call  main", 0xa, "	mov   eax, esi			; exit code", 0xa, "call  sys~exit			; call exit", 0xa, "main:", 0xa, 0
    mycelium#file_end                  db "mov	rsi, 0", 0xa, "ret", 0xa, 0

section .text

_start:
  mov   r15, rsp
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
    call    token#generate

    mov     rax, filename
    mov     rbx, file#flags#create | file#mode#read_write
    mov     rcx, file#perm#rw
    call    file~open

    mov     r10, rsi

    mov     rax, outfile
    mov     rbx, file#flags#create | file#mode#write
    mov     rcx, file#perm#rw
    call    file~open

    mov     r11, rsi

    mov     rax, r11
    call    mycelium#prime_file

    mov     rax, r10
    call    file~read_lines

    mov     rax, rsi
    call    lexer#lex_lines

	mov		rax, rsi
	call	arr~printn

    mov     rax, r11
    mov     rbx, rsi

    mov     rax, r11
    call    mycelium#end_file

    mov     rax, r10
    call    file~close


	mov		rsi, 0

    ret

; Args
;   rax: file
; Returns
;   void
mycelium#prime_file:
    push    r9                  ; file
    mov     r9, rax
    mov     rbx, mycelium#file_section_data
    call    file~write_cs

    ;; This is where we'll eventually put consts
    ;; We'll probably add a bss section also

    mov     rax, r9
    mov     rbx, mycelium#file_section_text
    call    file~write_cs


    pop     r9
    ret

; Args
;   rax: file
; Returns
;   void
mycelium#end_file:
    mov     rbx, mycelium#file_end
    call    file~write_cs

    ret



; Args
;   rax: file
;   rbx: array of ops
; Returns
;   void
broken#write_ops:
    push    r8                  ; counter
    push    r9                  ; current tuple
    push    r10                 ; file
    push    r11                 ; array of ops
    push    r12                 ; number of ops

    lea     r10, [rax]
    lea     r11, [rbx]

    mov     r12, [r11 + arr#meta#user_size]
    xor     r8, r8

    jmp     .loop_check
    .loop:
        lea     rax, [r11]
        mov     rbx, r8
        call    arr~get

        lea     r9, [rsi]
        mov     rax, rsi
        mov     rbx, 0
        call    tuple~get

        cmp     rsi, token#binary#math#plus#code
        je      .case_plus
        cmp     rsi, token#binary#math#minus#code
        je      .case_minus
        jmp     .default

        .case_plus:
            lea     rax, [r9]
            mov     rbx, 1
            call    tuple~get

            je      .plus_const_int


            .plus_const_int:
                lea     rax, [r10]
                mov     rbx, token#mov
                call    file~write_cs

                lea     rax, [r10]
                mov     rbx, token#rax
                call    file~write_cs

                lea     rax, [r10]
                mov     rbx, token#comma
                call    file~write_cs

                lea     rax, [r9]
                mov     rbx, 2
                call    tuple~get

                lea     rax, [rsp-32]
                mov     rbx, rsi
                call    int~to_cstring

                lea     rbx, [rsp-32]
                lea     rax, [r10]
                call    file~write_cs


                lea     rax, [rsp-32]
                mov     bl, 0xa
                mov     [rax], bl
                mov     bl, 0
                mov     [rax+1], bl
                call    file~write_cs

            jmp     .switch_end
        .case_minus:
            mov     rax, exception~runtime~not_implemented
            call    exception~runtime~throw
            jmp     .switch_end
        .default:
            mov     rax, exception~compiletime~bad_token
            call    exception~runtime~throw
        .switch_end:
        add     r8, r12
    .loop_check:
        cmp     r8, r12
        jl      .loop


    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret
