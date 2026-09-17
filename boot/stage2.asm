bits 32
org 0x8000

global _start
_start:
	cli


.hang:
	cli
	hlt
	jmp .hang
