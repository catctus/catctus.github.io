##Assambly 
*29 dec 2023*

I wanted to learn more about computers and I thought a good way would be to look into some assambly. <br>
A few years ago I did a MOOC course called *nand to tetris*, which was a really cool course building a simple computer<br>
from scratch. Building up ram and the processor from simple nand gates.
So a lot of things covered in this Atari 

`
; 8086
org 100h

jmp main

message: 
    db 'Hello World!', 0
print:
    mov ah, 0eh
._loop:
    lodsb
    cmp al, 0
    je .done
    int 10h
    jmp ._loop
.done:
    ret

main:
    mov si, message
    call print
ret
`

