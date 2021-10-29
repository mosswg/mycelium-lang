    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _Mycelium_std_arr_
%define _Mycelium_std_arr_

%include "std/exception.asm"
%include "std/type.asm"
%include "std/mem.asm"

    ;; Array archtecure
    ;; [memory size, user size, data type, data, data, etc...]
    ;;
    ;; Array vs. List
    ;; Similarities:
    ;;  Arrays and Lists are both consective byte of memory and store their sizes
    ;; Differences:
    ;;  Lists can have any type of element in any position where all elements in an arrays must have the same type.
    ;;  Lists can have multiple different types which store extra metadata for example 2d lists which store width and height.
    ;;  For these reasons array are both faster and more memory efficient but lists are generally easier to work with.

arr#meta#mem_size               equ 0*8          ; The size of the array in memory. This is different to user size because we're lying to the user
arr#meta#user_size              equ 1*8          ; The size of the array according to the user
arr#meta#type                   equ 2*8          ; The type of each element in the array. This is where lists and arrays differ.

arr#global_stride:              equ 100*8        ; The memory size of an array is always a multiple of this + the type specific metadata size
arr#meta_size:                  equ 3*8


; Args
;   rax: array pointer
;   rbx: data type
;   rcx: array size
; Returns
;   void
arr#populate_metadata:
    push    rsi
    push    rax
    push    r9
    mov     [rax+arr#meta#user_size], rcx               ; User size

    mov     [rax+arr#meta#type], rbx                    ; Array Type

    mov     r9, rax

    mov     rax, rbx
    call    type~sizeof

    mov     rax, rsi

    mul     rcx

    add     rax, arr#global_stride

    mov     [r9+arr#meta#mem_size], rax                ; Memory Size

    pop     r9
    pop     rax
    pop     rsi
    ret


; Args
;   rax: the size of the new array in elements
;   rbx: the type of the array
; Returns
;   rsi: pointer to the array
arr#new:
    push    rcx
    push    rdx

    mov     rcx, rax

    mov     rax, arr#global_stride          ; Load the stride into rax

    add     rax, arr#meta_size              ; Add the metadata size
    call    mem~allocate                    ; Create an array in memory with the size specified in global stride

    lea     rax, [rsi]
    call    arr#populate_metadata

    pop     rdx
    pop     rcx
    ret



; Args
;   rax: the type of the array
;   rbx: c array
;   rcx: length of c array
; Returns
;   rsi: pointer to the array
arr#new_ca:
    push    rdx
    push    r10                 ; Array
    push    rbx

    mov     rbx, rax

    mov     rax, arr#global_stride          ; Load the stride into rax

    add     rax, arr#meta_size              ; Add the metadata size
    call    mem~allocate                    ; Create an array in memory with the size specified in global stride

    lea     r10, [rsi]

    lea     rax, [rsi]
    call    arr#populate_metadata

    pop     rbx
    push    r9                  ; Counter

    jmp     .loop_check
    .loop:
        lea     rax, [r10]
        mov     rbx, r9
        mov     rcx, [rbx + r9]


    .loop_check:
        cmp     r9, rcx
        jl      .loop


    mov     rsi, r10
    pop     r9
    pop     r10
    pop     rdx
    ret

; Args
;   rax: arr 1
;   rbx: arr 2
; Returns
;   rsi: out arr
arr#concat:
    push        r8              ; loop counter
    push        r9              ; size
    push        r11             ; second array
    push        r12             ; out array

    mov         r11, rbx

    xor         r8, r8          ; clear r8
    mov         r9, [rbx + arr#meta#user_size]

    call        arr~copy        ; copy the first array

    mov         r12, rsi

    jmp         .loop_check
    .loop:
        mov     rax, r11
        mov     rbx, r8
        call    arr~get

        mov     rax, r12
        mov     rbx, rsi
        call    arr~push
    
        add     r8, 1
    .loop_check:
        cmp     r8, r9
        jl      .loop

    mov         rsi, r12

    pop         r12
    pop         r11
    pop         r9
    pop         r8
    ret

; Args
;   rax: arr 1 (also out array)
;   rbx: arr 2
; Returns
;   void
arr~concat:
    push        r8              ; loop counter
    push        r9              ; size
    push        r11             ; second array
    push        r12             ; out array

    mov         r11, rbx
    mov         r12, rax

    xor         r8, r8          ; clear r8
    mov         r9, [rbx + arr#meta#user_size]

    jmp         .loop_check
    .loop:
        mov     rax, r11
        mov     rbx, r8
        call    arr~get

        mov     rax, r12
        mov     rbx, rsi
        call    arr~push

        add     r8, 1
    .loop_check:
        cmp     r8, r9
        jl      .loop

    mov         rsi, r12

    pop         r12
    pop         r11
    pop         r9
    pop         r8
    ret


; Args
;   rax: arr
; Returns
;   rsi: copied arr
arr~copy:
    push        r8              ; loop counter
    push        r10             ; starting array
    push        r11             ; out array
    push        r12             ; size

    mov         r10, rax
    mov         r12, [rax + arr#meta#user_size]

    xor         r8, r8          ; clear r8

    mov         rbx, [rax + arr#meta#type]
    mov         rax, 0

    call        arr#new
    mov         r11, rsi

    .loop:
        mov     rax, r10
        mov     rbx, r8
        call    arr~get

        mov     rax, r11
        mov     rbx, rsi
        call    arr~push

        add     r8, 1
    .loop_check:
        cmp     r8, r12
        jl      .loop

    mov         rsi, r11

    pop         r12
    pop         r11
    pop         r10
    pop         r8
    ret

; Args
;   rax: pointer to the array
;   rbx: check value
; Returns
;   zf: If the array contains the value
arr~contains:
    push    r9                  ; Counter
    push    r10                 ; Array
    push    r11                 ; Length
    push    r12                 ; Compare Value
    push    r13                 ; Type

    xor     r9, r9
    lea     r10, [rax]
    mov     r11, [r10+arr#meta#user_size]
    mov     r12, rbx
    mov     r13, [r10+arr#meta#type]


    jmp  .loop_check
    .loop:
        mov     rax, r10
        mov     rbx, r9
        call    arr~get

        mov     rax, r13
        mov     rbx, rsi
        mov     rcx, r12

        call    type~compare
        je      .return


        add     r9, 1
    .loop_check:
        cmp     r9, r11
        jl      .loop

    mov     rax, 1
    cmp     rax, 0                ; If we get here we want to ensure that the zero flag is set correctly

    .return:
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    ret



; Args
;   rax: pointer to the array
;   rbx: compare array
; Returns
;   zf: If the array contains the value
arr~compare:
    push    r8                  ; Temp value
    push    r9                  ; Counter
    push    r10                 ; Array
    push    r11                 ; Length
    push    r12                 ; Compare array
    push    r13                 ; Type

    xor     r9, r9
    lea     r10, [rax]
    mov     r11, [r10+arr#meta#user_size]
    mov     r12, rbx
    mov     r13, [r10+arr#meta#type]

    mov     rax, r11
    mov     rbx, [r12 + arr#meta#user_size]
    cmp     rax, rbx
    jne     .return             ; If the sizes are different don't even check the values

    jmp  .loop_check
    .loop:
        lea     rax, [r10]
        mov     rbx, r9
        call    arr~get

        mov     r8, rsi

        lea     rax, [r12]
        mov     rbx, r9
        call    arr~get

        mov     rax, r13
        mov     rbx, r8
        mov     rcx, rsi

        call    type~compare
        jne     .not_equal

        add     r9, 1
    .loop_check:
        cmp     r9, r11
        jl      .loop

    jmp     .equal
    .not_equal:
        mov     r13, 1
        cmp     r13, 0                ; If we get here we want to ensure that the zero flag is set correctly
        jmp     .return
    .equal:
        mov     r13, 1
        cmp     r13, 1                ; If we get here we want to ensure that the zero flag is set correctly
    .return:
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    ret




; Args
;   rax: pointer to the array
; Returns
;   void
arr~del:
    mov     rbx, [rax]
    call    mem~deallocate
    ret


; Args
;   rax: pointer to the array
; Returns
;   void
arr~print_string:
    .string:    db ", ", 0x0
arr~print:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    r9
    push    r10
    push    r11
    mov     r10, rax
    mov     r11, rbx
    mov     rdx, [r10+arr#meta#type]        ; Get the type into rax
    mov     rax, rdx
    call    type~sizeof         ; Get the size of each element into rsi
    mov     r9, rsi             ; Move the size to a usable location

    lea     rcx, [r10]

    push    rdx

    mov     rax, [r10+arr#meta#user_size]          ; Get length
    mul     r9

    pop     rdx

    add     rcx, rax            ; Get the end address

    mov     rax, '['
    call    out~putc

    lea     r10, [r10+arr#meta_size]       ; Offset to account for size and type variables
    lea     rcx, [rcx+arr#meta_size]       ; Offset the end address too

    mov rax, [r10]
    mov rbx, rdx
    call type~print

    add     r10, r9            ; Increment the index

    jmp     .loop_check
    .loop:
        mov     rax, arr~print_string
        mov     rbx, 2
        call    out~puts


        mov     rax, [r10]
        mov     rbx, rdx
        call    type~print

        add     r10, r9            ; Increment the index
    .loop_check:
        cmp     r10, rcx
        jl     .loop


    mov     rax, ']'
    call    out~putc

    pop     r11
    pop     r10
    pop     r9
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax                 ; Perserve the pointer to the start of the array
    ret

; Args
;   rax: pointer to the array
; Returns
;   void
arr~println:
    push    rax

    call    arr~print

    mov     rax, 0xA
    call    out~putc

    pop     rax
    ret

; Args
;   rax: pointer to the array
; Returns
;   void
arr~printn:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    r9
    push    r10
    mov     r10, rax
    mov     rdx, [r10+arr#meta#type]        ; Get the type into rax
    mov     rax, rdx
    call    type~sizeof         ; Get the size of each element into rsi
    mov     r9, rsi             ; Move the size to a usable location

    lea     rcx, [r10]

    push    rdx

    mov     rax, [r10+arr#meta#user_size]          ; Get length
    mul     r9

    pop     rdx

    add     rcx, rax            ; Get the end address

    lea     r10, [r10+arr#meta_size]       ; Offset by 16 to account for size and type variables
    lea     rcx, [rcx+arr#meta_size]       ; Offset the end address too

    jmp     .loop_check
    .loop:
        mov rax, [r10]
        mov rbx, rdx
        call type~print

        mov     rax, 0xA
        call    out~putc

        add     r10, r9            ; Increment the index
    .loop_check:
        cmp     r10, rcx
        jl     .loop

    pop     r10
    pop     r9
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax                 ; Perserve the pointer to the start of the array
    ret




; Args
;   rax: pointer to the array
; Returns
;   rsi: length of the array
arr~len:
    mov     rsi, [rax]
    ret


; Args
;   rax: pointer to the array
;   rbx: index of the element
; Returns
;   rsi: element
arr~get:
    mov     rsi, [rax+arr#meta#user_size]
    cmp     rbx, rsi
    jle     .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax
    push    rax
    mov     rax, [rax+arr#meta#type]        ; Get the type into rax
    call    type~sizeof         ; Get the size of each element into rsi

    mov     rax, rbx
    mul     rsi                 ; Get the byte index of the element
    mov     rsi, rax
    pop     rax
    add     rax, rsi

    mov     rsi, [rax+arr#meta_size]       ; Offset by the size of the metadata to account for size and type variables

    pop     rax                 ; Perserve the pointer to the start of the array
    ret


; Args
;   rax: pointer to the array
;   rbx: index of the element
;   rcx: write value
; Returns
;   void
arr~set:
    mov     rsi, [rax]          ; Get size
    cmp     rsi, rbx            ; Validate Index
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax
    push    rax
    mov     rax, [rax+arr#meta#type]        ; Get the type into rax
    call    type~sizeof         ; Get the size of each element into rsi

    mov     rax, rbx
    mul     rsi                 ; Get the byte index of the element
    mov     rsi, rax
    pop     rax
    add     rax, rsi            ; Get move the index minus 16

    mov     [rax+arr#meta_size], rcx       ; Offset by 16 to account for size and type variables

    pop     rax                 ; Perserve the pointer to the start of the array
    ret



; Args
;   rax: pointer to the array
;   rbx: new size
; Returns
;   rax: new pointer if needed
arr~resize:
    push    r9
    push    rdx
    push    rbx
    push    rax

    mov     r9, [rax+arr#meta#mem_size]
    mov     rdx, rbx
    shl     rdx, 3              ; Multiply by 8, the size of one element

    pop     rax
    add     rdx, arr#meta_size

    cmp     rdx, r9
    pop     rbx
    jl      .good_size

    push    r10
    mov     r10d, 100
    push    rbx
    push    rcx
    push    rax

    mov     rax, rbx
    shl     rax, 3              ; Multiply by 3 to get the byte size
    mov     rdx, 0
    div     r10d

    add     rax, 1              ; Add one to result because integer division always rounds down when there is a remainder and it's more of an issue to have too little than too much

    mul     r10d                 ; Get the new size in an multiple of 100

    add     rax, rsi            ; Add the amount of metadata

    mov     rcx, rax
    pop     rax
    mov     rbx, [rax+arr#meta#mem_size]

    mov     [rax+arr#meta#mem_size], rcx

    call    mem~reallocate

    pop     rcx
    pop     rbx
    pop     r10
    .good_size:


    mov     [rax+arr#meta#user_size], rbx

    pop     rdx
    pop     r9
    ret

; Args
;   rax: pointer to the array
;   rbx: the amount increase size by
; Returns
;   rax: new address pointer if needed
;   rsi: the new size
arr~increase_size:
    push    rbx

    mov     rsi, [rax+arr#meta#user_size]
    add     rsi, rbx

    mov     rbx, rsi
    call    arr~resize

    pop     rbx
    ret

; Args
;   rax: pointer to the array
;   rbx: value to push
; Returns
;   rax: the new pointer if needed
arr~push:
    push    r9
    push    r10
    push    rcx

    mov     r9, rbx             ; Store value to push in temp place
    mov     rbx, 1              ; Increase the size by 1
    call    arr~increase_size

    mov     rbx, rsi            ; Put the new size as the index for set
    mov     rcx, r9             ; Put the value as the push value to write the end of the array
    sub     rbx, 1
    call    arr~set            ; Set the value

    pop     rcx
    pop     r10
    pop     r9
    ret

; Args
;   rax: pointer to the array
; Returns
;   rsi: value
arr~pop:
    push    rbx
    push    rcx
    mov     rbx, [rax+arr#meta#user_size] ; Load the size of the list into rbx

    sub     rbx, 1

    call    arr~get             ; rax.get(rbx)
    mov     [rax+arr#meta#user_size], rbx

    pop     rcx
    pop     rbx
    ret



; Args
;   rax: pointer to the array
;   rbx: Function
;   rcx: Return type
;   rdx: arg 1
;   r8:  arg 2
; Returns
;   rsi: Array of outputs
arr~for_each:
    push    r9                  ; Array of outputs
    push    r10                 ; First arg of the called function
    push    r11                 ; Original Array Pointer
    push    r12                 ; Counter
    push    r13                 ; Pointer to after the metadata
    push    r14                 ; Size of the array
    push    rbx


    lea     r11, [rax]
    mov     r14, [rax + arr#meta#user_size]
    lea     r13, [rax + arr#meta_size]
    xor     r12, r12

    mov     rax, 0
    mov     rbx, rcx

    call    arr#new

    lea     r9, [rsi]

    ;; Move args for the functions
    mov     r10, rdx
    mov     rcx, r8

    xor     rsi, rsi


    .loop:
        mov     rax, r12

        lea     rax, [r11]
        mov     rbx, r12

        call    arr~get

        mov     rax, rsi

        mov     rbx, r10
    
        call    [rsp]

        mov     rbx, rsi
        lea     rax, [r9]

        call    arr~push

        add     r12, 1
    .loop_check:
        cmp     r12, r14
        jl      .loop

    lea     rsi, [r9]

    pop     rbx
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    ret


%endif                          ; ifdef guard
