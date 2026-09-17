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

	lea     rdi, [rel __bss_start] ; 64‑bit RIP‑relative load
	xor     rax, rax   ; rax = 0
	mov     rcx, __bss_end
	sub     rcx, rdi     ; rcx = size in bytes
	shr     rcx, 3     ; divide by 8 → # of qwords
	rep     stosq   ; write 8‑byte zeros

	mov     rcx, __bss_end
	sub     rcx, rdi ; total byte count again
	and     rcx, 7  ; remaining 0‑7 bytes
	jz      .bss_done
	rep     stosb ; store remaining bytes
.bss_done:

	call kmain
;infinite loop!
.hang:
	cli
	hlt
	jmp .hang
