[bits 16]
[org 0x7c00]

KERNEL equ 0x8000

global start
section .text

start:
    jmp _start
    nop

times 33 db 0 ;BPB reservation space

_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x6000 ;stack pointer, personal reasons for not 0x7c00

    mov [boot_drive], dl

    mov ax, 0x0003
    int 0x10

    mov si, msg
    call printb

    cli
    hlt
    jmp $-2

printb:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp printb

.done:
    ret

;comment

msg db "UROS Bootloader!", 13, 10, 0
boot_drive db 0

times 512 - ($ - $$) db 0
dw 0xaa55
