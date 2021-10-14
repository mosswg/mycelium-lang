%ifndef _sys_platform_asm_
%define _sys_platform_asm_

  sys~fd~in:               equ 0x00
  sys~fd~out:              equ 0x01
  sys~fd~err:              equ 0x02


  sys~id~restart_syscall:  equ 0x00
  sys~id~exit:             equ 0x01
  sys~id~fork:             equ 0x02
  sys~id~read:             equ 0x03
  sys~id~write:            equ 0x04
  sys~id~open:             equ 0x05
  sys~id~close:            equ 0x06
  sys~id~waitpid:          equ 0x07
  sys~id~creat:            equ 0x08
  sys~id~link:             equ 0x09
  sys~id~unlink:           equ 0x0A
  sys~id~execve:           equ 0x0B
  sys~id~chdir:            equ 0x0C
  sys~id~time:             equ 0x0D
  sys~id~mknod:            equ 0x0E
  sys~id~chmod:            equ 0x0F
  sys~id~lchown16          equ 0x10
; not implemented       equ 0x11
  sys~id~stat:             equ 0x12
  sys~id~lseek:            equ 0x13

; Args
;   rax: fd
;   rdi: buffer
;   rsi: length
sys~write:
  push  rdx
  mov   rdx, rsi
  mov   rcx, rdi
  mov   rbx, rax
  mov   rax, sys~id~write
  int 0x80
  pop rdx
  ret

; Args
;   rax: exit code

sys~exit:
  push  rbx
  mov   rbx, rax
  mov   rax, sys~id~exit
  int 0x80
  pop   rbx
  ret

%endif
