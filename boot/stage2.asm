bits 32
org 0x8000

global _start
_start:
	cli
	mov byte [0xB8000], 'S'
	mov byte [0xB8001], 0x4D
	mov byte [0xB8002], '2'
	mov byte [0xB8003], 0x4D

.hang:
	cli
	hlt
	jmp .hang
