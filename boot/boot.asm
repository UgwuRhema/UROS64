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

	mov [boot_drive], dl

	mov ax, 0x0003
	int 0x10

	mov si, intro
	call printb

	call enable_a20

	call read_disk ;reads the disk and loads the kernel into memory
	
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
	dw 120       ;120 sectors is a lot
	dw KERNEL ;the magic number we made earlier, this is what we would jump to when we enter long mode
	dw 0 ;segment?
	dq 1   ;starting LBS(Logical Block Sector), its the sector right after the boot sector

intro db "UROS Bootloader", 13, 10, 0
boot_drive db 0
err_msg db "Failed to Read disk! Halting...", 13,10,0

times 510 - ($ - $$) db 0
dw 0xaa55
