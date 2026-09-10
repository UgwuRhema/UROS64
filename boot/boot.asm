bits 16
org 0x7c00

KERNEL_EQU equ 0x8000 ;magic number, this is the address i would like to load our kernel in

global start
section .text

start:
  jmp short _start
  nop

times 33 db 0 ;BPB reservation space

_start:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x6000 ;yes my stack pointer in lower

	mov ax, 0x0003
	int 0x10

	mov si, intro
	call printb

	call enable_a20
	
	cli
	hlt
	jmp $-2

printb:
	lodsb
	cmp al, 0
	je .done
	mov ah, 0x0e
	int 0x10
	jmp printb ;recursive printing
.done:
	ret

enable_a20:
	in al, 0x92
	or al, 0x02
	and al, 0xfe
	out 0x92, al
	ret	

intro db "UROS Bootloader", 13, 10, 0

times 510 - ($ - $$) db 0
dw 0xaa55
