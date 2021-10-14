%ifndef _sys_platform_asm_
%define _sys_platform_asm_

  sys~fd~in:                equ 0x0
  sys~fd~out:               equ 0x1
  sys~fd~err:               equ 0x2

  sys~id~read:              equ 0x00
  sys~id~write:             equ 0x01
  sys~id~open:              equ 0x02
  sys~id~close:             equ 0x03
  sys~id~stat:              equ 0x04
  sys~id~fstat:             equ 0x05
  sys~id~lstat:             equ 0x06
  sys~id~poll:              equ 0x07
  sys~id~lseek:             equ 0x08
  sys~id~mmap:              equ 0x09
  sys~id~mprotect:          equ 0x0A
  sys~id~munmap:            equ 0x0B
  sys~id~brk:               equ 0x0C

  sys~id~exit:              equ 0x3C

; Args
;   rax: file descriptor
;   rdi: output buffer
;   rsi: length of the output buffer
; Returns
;   void
sys~write:
  push  rdx
  
  mov   rdx, rsi
  mov   rsi, rdi
  mov   rdi, rax
  mov   rax, sys~id~write
  syscall

  mov   rax, rdi
  mov   rdi, rsi
  mov   rsi, rdx

  pop   rdx
  ret

; Args
;   rax: exit code
; Returns
;   void
sys~exit:
  mov   rbx, rax
  mov   rax, sys~id~exit
  syscall
  ret



%endif
