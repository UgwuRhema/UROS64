bits 64
org 0x0

section .text

global _begin
_begin:
	cld ;clear the descriptor flag...
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	;let's print something to verfiy successful jump
	mov byte [0xb8000], 'K'
	mov byte [0xb8001], 0x0d
	mov byte [0xb8002], 'M'
	mov byte [0xb8003], 0x0d

;infinite loop!
.hang:
	cli
	hlt
	jmp .hang
