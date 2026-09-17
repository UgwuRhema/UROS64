bits 32
org 0x8000

global _start
_start:
	cli
	mov byte [0xB8000], 'P'
	mov byte [0xB8001], 0x3F

.hang:
	cli
	hlt
	jmp .hang
