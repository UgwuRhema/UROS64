bits 32
org 0x8000

KERNEL equ 0x10000 ;kernel address magic number
SIZE_OF_PAGE_TABLES equ 3072 ;size of Page tables, i believe the name makes it clear
PML equ 0x70000

global _start
_start:
	cli
	mov ebp, 0x90000
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, ebp ;stack pointer

	;building page tables at 0x10000
	mov edi, PML
	mov cr3, edi  ;this let's the CPU know where page tables are
	 
	xor eax, eax
	mov ecx, SIZE_OF_PAGE_TABLES
	rep  stosd   ;writes 4 * SIZE_OF_PAGE_TABLES, which is enough space...
	mov edi, cr3

	mov dword [0x70000], 0x21003
	mov dword [0x71000], 0x22003
	mov dword [0x72000], 0x00000083

	; Enable PAE
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	;ENable Long Mode in EFER MSR
	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	;enable paging
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	;load GDT and far Jump
	lgdt [gdt64_desc]
	jmp 0x08:long_mode

;64 bit Global Descriptor Table
gdt64_start:
	dq 0x0000000000000000
gdt64_code:
	dq 0x002090a000000000  ; L=1, D=0
gdt64_data:
	dq 0x0000920000000000
gdt64_end:

gdt64_desc:
	dw gdt64_end - gdt64_start - 1
	dd gdt64_start

bits 64
long_mode:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov rsp, 0x90000

	jmp KERNEL
