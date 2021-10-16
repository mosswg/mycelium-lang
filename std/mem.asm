    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _mxs_std_mem_
%define _mxs_std_mem_

%include "std/sys.asm"
%include "std/exception.asm"

; Args
;   rax: amount of memory in bytes
; Returns
;   rsi: pointer to allocated memory
mem~allocate:
    mov     rsi, rax            ; len = rax
    mov     rax, sys~id~mmap
    xor     rdi, rdi            ; addr = NULL
    mov     rdx, 7              ; prot = PROT_READ|PROT_WRITE
    mov     r10, 0x22           ; flags = MAP_PRIVATE|MAP_ANONYMOUS
    mov     r8, -1              ; fd = -1
    xor     r9, r9              ; offset = 0 (4096*0)
    syscall                 ; make call
    mov     rsi, rax
    ret

; Args
;   rax: pointer to the buffer
;   rbx: length of the buffer
; Returns
;   void
mem~deallocate:
    mov     rdi, rax
    mov     rsi, rbx
    mov     rax, sys~id~munmap
    syscall
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
