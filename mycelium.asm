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
    filename:       db "examples/oper.myc", 0
    outfile:        db "oper.myc.asm", 0


    mycelium#file_section_data:        db '%include "std/sys.asm"', 0xa, 0xa, "global _start", 0xa, 0xa, "section .data", 0xa, 0
    mycelium#file_section_text:        db "section .text", 0xa, 0xa, "_start:", 0xa, "	mov   r15, rsp", 0xa, "	call  main", 0xa, "	mov   eax, esi			; exit code", 0xa, "	call  sys~exit			; call exit", 0xa, 0xa, "main:", 0xa, 0
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

	mov	rax, rsi
    ;; call	arr~printn

    mov     rax, r11
    mov     rbx, rsi

    mov     rax, r11
    call    mycelium#end_file

    mov     rax, r10
    call    file~close

    .tokendel:

    call    token#del

    .loop:
    jmp     .loop


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
;   rax: array of tokens
; Returns
;   rsi: array of ops
mycelium#parse:
    push    r8                  ; counter
    push    r9                  ; current tuple
    push    r10                 ; file
    push    r11                 ; array of tokens
    push    r12                 ; number of tokens
    push    r13                 ; array of ops

    lea     r10, [rax]
    lea     r11, [rbx]

    mov     r12, [r11 + arr#meta#user_size]
    xor     r8, r8

    mov     rax, 0
    mov     rbx, type#tuple
    call    arr#new

    mov     r13, rsi

    jmp     .loop_check
    .loop:
        mov     rax, r11
        mov     rbx, r8
        call    arr~get

        mov     r9, rsi




        add     r8, r12
    .loop_check:
        cmp     r8, r12
        jl      .loop

    mov     rsi, r13

    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret
