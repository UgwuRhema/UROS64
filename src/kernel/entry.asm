bits 64

section .text

extern __bss_start, __bss_end
extern kmain

global _begin
_begin:
	cld ;clear the descriptor flag...
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	call kmain
;infinite loop!
.hang:
	cli
	hlt
	jmp .hang
