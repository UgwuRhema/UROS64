bits 16
org 0x7c00

STAGE2 equ 0x8000 ;magic number, this is the address i would like to load our kernel in

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

	mov [boot_drive], dl

	mov ax, 0x0003
	int 0x10

	mov si, intro ;greeter message
	call printb

	call enable_a20 ;fast a20

	;we will now detect memory via BIOS using the SMAP method
	call detect_memory

	call read_disk ;reads the disk and loads the kernel into memory

	lgdt [gdt_desc]

	cli
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp 0x08:protected
	
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

;this enales fast a20
enable_a20:
	in al, 0x92
	or al, 0x02
	and al, 0xfe
	out 0x92, al
	ret

read_disk:
	;let's use the extende LBA read then, really i forgot
	mov si, dap
	mov ah, 0x42
	mov dl, [boot_drive]
	int 0x13
	jc .err
	ret

.err:
	mov si, err_msg
	call printb
	cli
	hlt ;halt CPU
	jmp $-2

dap:
	db 0x10 ;packet size, 16 bytes
	db 0 ; reserved, must be 0

dap_count:
	dw 2       ;only 2 sectors
	dw STAGE2 ;the magic number we made earlier, this is what we would jump to when we enter long mode
	dw 0 ;segment?
	dq 1   ;starting LBS(Logical Block Sector), its the sector right after the boot sector

;memory constants, magic numbers
NUM_MEMORY_ENTRIES_PTR equ 0x1000
MEMORY_MAP_BUFFER_PTR equ 0x1004

detect_memory:
	pushad ;save all registers safely
	xor ebx, ebx
	mov edx, 0x534d4150 ;magic number 'SMAP'
	mov di, MEMORY_MAP_BUFFER_PTR

	;zero out initial counter slot
	mov dword [NUM_MEMORY_ENTRIES_PTR], 0

.mem_loop:
	mov eax, 0xe820 ;bios function request map!
	mov ecx, 24 ;request a 24-byte entry structure
	int 0x15
	jc .mem_done ;if carry flag is set, list is complete

	cmp eax, 0x534d4150 ;verify CPU responded with 'SMAP'
	jne .mem_done

	add di, 24 ;incerment buffer pointer foward
	inc dword [NUM_MEMORY_ENTRIES_PTR] ;increment map entry counter

	test ebx, ebx ;if ebx is 0, parsing is done
	jne .mem_loop

.mem_done:
	popad ;as you know what this does, restore all registers
	ret

bits 32
protected:
	cli
	mov ebp, 0x90000
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, ebp

	;mov byte [0xB8000], 'P'
	;mov byte [0xB8001], 'M'
	;mov byte [0xB8002], 'M'
	;mov byte [0xB8003], 'M'

	mov eax, STAGE2
	jmp eax

gdt_start:
	gdt_null:
		;null descriptor, always zero....
		dd 0x00000000
		dd 0x00000000
	gdt_code:
		dw 0xffff
		dw 0x0000
		db 0x00
		db 0b10011010
		db 0b11001111
		db 0x00
	gdt_data:
		dw 0xffff
		dw 0x0000
		db 0x00
		db 0b10010010
		db 0b11001111
		db 0x00
gdt_end:

gdt_desc:
	dw gdt_end - gdt_start - 1
	dd gdt_start

;magic number section
CODE_SEL equ 0x08
DATA_SEL equ 0x10

intro db "UROS Bootloader", 13, 10, 0
boot_drive db 0
err_msg db "Failed to Read disk! Halting...", 0

times 510 - ($ - $$) db 0
dw 0xaa55
