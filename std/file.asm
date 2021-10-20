
    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_file_
%define _Mycelium_std_file_

%include "std/sys.asm"
%include "std/exception.asm"
%include "std/mem.asm"
%include "std/type.asm"

file#mode#read:       equ 0x0
file#mode#write:      equ 0x1
file#mode#read_write:      equ 0x2
file#mode#access_mode:     equ 0x3

file#flags#create:          equ 0x40
file#flags#excl:            equ 0x80
file#flags#no_ctty:         equ 0x100
file#flags#truncate:        equ 0x200
file#flags#append:          equ 0x400
file#flags#nonblock:        equ 0x800
file#flags#sync:            equ 0x4040000
file#flags#async:           equ 0x2000
file#flags#large:           equ 0x200000
file#flags#directory:       equ 0x400000
file#flags#no_follow:       equ 0x800000
file#flags#close_on_exec:   equ 0x2000000
file#flags#direct:          equ 0x100000
file#flags#no_atime:        equ 0x40000
file#flags#path:            equ 0x200000
file#flags#dsync:           equ 0x1000
file#flags#temp_file:       equ (0x400000 | file#flags#directory)

file#perm#all:              equ 0x1ff
file#perm#rw:               equ 0x1a4


file#stat#size:             equ 144


section .bss
    tmp_stats:   resb file#stat#size

struc stats
    .dev         resq 1
    .ino         resq 1
    .nlink       resq 1
    .mode        resd 1
    .uid         resd 1
    .gid         resd 1
    .pad0        resb 4
    .rdev        resq 1
    .size        resq 1
    .blksize     resq 1
    .blocks      resq 1
    .atime       resq 1
    .atime_nsec  resq 1
    .mtime       resq 1
    .mtime_nsec  resq 1
    .ctime       resq 1
    .ctime_nsec  resq 1
endstruc



    ;; File archtecure
    ;; [File path, fd, file stats, data, data, etc...]

file#meta#path:     equ 0*8
file#meta#fd:       equ 1*8
file#meta#stats:    equ 2*8
file#data_offset:   equ file#meta#stats + file#stat#size + 1*8

section .text

; Args
;   rax: File path
;   rbx: Flags
;   rcx: Mode
; Returns
;   rsi: pointer to allocated memory
file~open:
    push    r8
    push    rax

    mov     rax, file#data_offset + 1*8

    call    mem~allocate
    mov     r8, rsi

    mov     rax, [rsp]

    mov     [r8+file#meta#path], rax

    mov     rdx, rcx
    mov     rsi, rbx
    mov     rdi, rax
    mov     rax, sys~id~open
    syscall
    mov     [r8+file#meta#fd], rax

    mov     rax, r8
    call    file~stats

    lea     rax, [r8]
    call    file~mmap

    mov     rsi, r8
    pop     rax
    pop     r8
    ret

; Args
;   rax: File path
; Returns
;   rsi: stat object
file#stats:
    push    rax

    mov     rdi, rax
    lea     rsi, tmp_stats
    mov     rax, sys~id~stat
    syscall

    pop     rax
    ret


; Args
;   rax: File object
; Returns
;   rsi: Array of lines
file~read_lines:
    push    r8                  ; File object
    push    r9                  ; Array
    push    r14                 ; File data array
    push    r10                 ; Loop Counter
    push    r11                 ; Current line
    push    r12                 ; Current line counter
    push    r13                 ; Current char


    mov     r8, rax             ; Store rax for later use

    mov     r14, [rax+file#data_offset]

    mov     rax, 1
    mov     rbx, type#string
    call    arr#new             ; Create array of length 1 to push the lines of the file to

    mov     r9, rsi

    xor     r10, r10            ; Clear r10
    xor     r11, r11            ; Clear r11


    mov     r11, [r9]



    .loop:
        mov     r13b, [r14+r10]

        cmp     r13b, 0xA   ; Check if it's a new line character
        jne     .current_line

        xor     r11, r11        ; Clear current line count

        add     r12, 1

        mov     rax, r9

        mov     rbx, 0

        call    arr~push

        .current_line:
        add     r10, 1
    .loop_check:
        cmp     r10, [r8+file#meta#stats + stats.size]
        jl      .loop


    mov     rsi, r9
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret






; Args
;   rax: File object
; Returns
;   void
file~stats:
    push    rax
    push    rdi

    mov     rdi, [rax+file#meta#fd]
    lea     rsi, [rax+file#meta#stats]
    mov     rax, sys~id~fstat
    syscall

    pop     rdx
    pop     rax
    ret


; Args
;   rax: file descriptor
; Returns
;   void
file~close:
    mov     rdi, rax
    mov     rax, sys~id~close
    syscall
    ret


; Args
;   rax: File object
; Returns
;   void
file~mmap:
    push    rcx
    push    r8
    push    r9
    push    rax

    mov     rsi, [rax + file#meta#stats + stats.size]   ; len = file.stats.size
    xor     rdi, rdi                                    ; addr = NULL
    mov     rdx, 7                                      ; prot = PROT_READ|PROT_WRITE
    mov     r10, mmap#private                           ; flags = MAP_PRIVATE|MAP_ANONYMOUS
    mov     r8, [rax + file#meta#fd]                    ; fd = file.fd
    xor     r9, r9                                      ; offset = 0 (4096*0)
    mov     rax, sys~id~mmap
    syscall                                             ; make call

    mov     r8, rax

    pop     rax
    mov     [rax+file#data_offset], r8
    pop     r9
    pop     r8
    pop     rcx

    ret

%endif                          ; ifdef guard
