bits 32
org 0x8000

KERNEL equ 0x10000 ;kernel address magic number
SIZE_OF_PAGE_TABLES equ 6144 ;size of Page tables, i believe the name makes it clear
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

	; PML4 -> PDPT
	mov dword [0x70000], 0x71003

	; PDPT -> 4 Page Directories (1GB each)
	mov dword [0x71000], 0x72003 ; 0 - 1GB
	mov dword [0x71008], 0x73003 ; 1 - 2GB
	mov dword [0x71010], 0x74003 ; 2 - 3GB
	mov dword [0x71018], 0x75003 ; 3 - 4GB (Covers LAPIC at 0xFEE00000)

	; Fill 2048 Page Directory entries (2048 * 2MB = 4GB)
	mov edi, 0x72000
	mov eax, 0x00000083          ; Present + Writable + 2MB Page Bit
	mov ecx, 2048                ; Total 2MB pages to map

	.map_4gb_loop:
	mov dword [edi], eax
	mov dword [edi + 4], 0       ; High 32 bits
	add eax, 0x200000            ; Advance physical base address by 2MB
	add edi, 8                   ; Next 64-bit table entry
	loop .map_4gb_loop	

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
	dq 0x00209a0000000000  ; L=1, D=0
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
