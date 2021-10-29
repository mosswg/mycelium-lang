    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_mem_
%define _Mycelium_std_mem_

%include "std/sys.asm"
%include "std/exception.asm"


mmap#shared:            equ 0x1
mmap#private:           equ 0x2
mmap#shared_validate:   equ 0x3

; Args
;   rax: amount of memory in bytes
; Returns
;   rsi: pointer to allocated memory
mem~allocate:
    push    rcx
    push    r8
    push    r9
    push    r10
    push    r11

    mov     rsi, rax                    ; len = rax
    mov     rax, sys~id~mmap
    xor     rdi, rdi                    ; addr = NULL
    mov     rdx, 7                      ; prot = PROT_READ|PROT_WRITE
    mov     r10, mmap#private | 0x20    ; flags = MAP_PRIVATE|MAP_ANON
    mov     r8, -1                      ; fd = -1
    xor     r9, r9                      ; offset = 0 (4096*0)
    syscall                             ; make call
    mov     rsi, rax

    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rcx
    ret

; Args
;   rax: pointer to the buffer
;   rbx: length of the buffer
; Returns
;   void
mem~deallocate:
    push    r11

    mov     rdi, rax
    mov     rsi, rbx
    mov     rax, sys~id~munmap
    syscall

    pop     r11
    ret


; Args
;   rax: pointer to the current buffer
;   rbx: current size
;   rcx: new size
; Returns
;   rax: new pointer (same as rax if not moved)
mem~reallocate:
    mov     rdi, rax
    mov     rsi, rbx
    mov     rdx, rcx
    mov     r10, 0x1            ; Flags = MREMAP_MAYMOVE
    mov     rax, sys~id~mremap
    syscall
    cmp     rax, 0
    jg      .return

    mov     rax, exception~runtime~bad_allocation
    call    exception~runtime~throw

    .return:
    ret

%endif                          ; ifdef guard
