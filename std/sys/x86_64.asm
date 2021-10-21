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
  sys~id~rt_sigaction:      equ 0x0D
  sys~id~rt_sigprocmask:    equ 0x0E
  sys~id~rt_sigreturn:      equ 0x0F
  sys~id~ioctl:             equ 0x10
  sys~id~pread64:           equ 0x11
  sys~id~pwrite64:          equ 0x12
  sys~id~readv:             equ 0x13
  sys~id~writev:            equ 0x14
  sys~id~access:            equ 0x15
  sys~id~pipe:              equ 0x16
  sys~id~select:            equ 0x17
  sys~id~sched_yield:       equ 0x18
  sys~id~mremap:            equ 0x19
  sys~id~msync:             equ 0x20

  sys~id~exit:              equ 0x3C

  sys~id~unlink:            equ 0x57
; Args
;   rax: file descriptor
;   rdi: output buffer
;   rsi: length of the output buffer
; Returns
;   void
sys~write:
  push  rdx
  push  r11
  
  mov   rdx, rsi
  mov   rsi, rdi
  mov   rdi, rax
  mov   rax, sys~id~write
  syscall

  mov   rax, rdi
  mov   rdi, rsi
  mov   rsi, rdx

  pop   r11
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
