  ;; Author: Moss Gallagher
  ;; Date: 12-Oct-21

%include "std/list.asm"
%include "std/arr.asm"
%include "std/str.asm"
%include "std/int.asm"
%include "std/type.asm"


global _start

section .data
  create_success:     db "Created list with size ", 0

  inner_str:          db "String", 0

  first_size:         equ 8
  inner_size:         equ 4

section .text

_start:
  mov   r15, rsp
  call  main
  mov   eax, esi                ; exit code
  call  sys~exit                ; call exit

main:
  mov   rax, list#type#basic
  mov   rbx, (first_size/2)

  call  list#new                ; Create list

  mov   r9, rsi                 ; Store the created list in r9


  mov   rax, type#int
  mov   rbx, inner_size         ; Create array with size inner_size and type int

  call  arr#new


  mov   rax, r9                 ; Retrieve list
  mov   rbx, rsi                ; Tell the list what we want to push
  mov   rcx, type#arr           ; Tell the list what we want to push is of type array

  call  list~push               ; Push the array


  mov   rax, r9                 ; Retrieve the list
  mov   rbx, first_size         ; Tell the list we want what we want to resize to

  call  list~resize             ; Resize the list


  mov   rax, r9                 ; Retrieve the list
  mov   rbx, inner_str          ; Tell the list what we want to push
  mov   rcx, type#string        ; Tell the list what we want to push is of type string

  call  list~push               ; Push the string


  mov   rax, r9                 ; Retrieve the list
  mov   rbx, first_size + (first_size/2) ; Tell the list we want to resize to the first size * 1.5

  call  list~resize             ; Resize the list


  mov   rax, r9                 ; Retrieve the list
  mov   rbx, 'a'                ; Tell the list what we want to push
  mov   rcx, type#char          ; Tell the list what we want to push is of type char

  call  list~push               ; Push the character


  mov   rax, r9                 ; Retrieve the list

  call  list~println            ; Print the list


  mov   rax, r9                 ; Retrieve the list

  call  list~del                ; Delete the list

  mov   rsi, 0
  ret
