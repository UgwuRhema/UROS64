bits 16
org 0x7c00 ;starting address in memory

KERNEL equ 0x8000

global start
section .text

start:
	jmp _start
	nop

times 33 db 0       ;BPB(BIOS PARAMETER BLOCK) reservation space

_start:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x6000

    mov [boot_drive], dl

	mov ax, 0x0003
	int 0x10

	mov si, msg
	call print_string

	;setting up the a20 gate,
	;now this is so that the bootloader can...
	;...access over 1MiB of RAM

	call enable_a20

    ;detect RAM via BIOS
    call detect_memory

	;reading the disk and loading the...
	;...kernel into memory

	call read_dsksec1

	mov al, 'D'
	call send_serial

    ;interactive Boot Prompt
    mov si, msg_prompt
    call print_string

.wait_key:
    sti     ;enable interrupts so keyboard input registers
    mov ah, 0x00
    int 0x16        ;wait for key press (Al = ASCII char)
    cli     ;Disbale interrupts again

    ;Check for 'B' or 'b' -> boot
    cmp al, 'b'
    je .do_boot
    cmp al, 'B'
    je .do_boot

    ;Ckeck for 'S' or 's' -> shutdown
    cmp al, 's'
    je .do_shutdown
    cmp al, 'S'
    je .do_shutdown

    jmp .wait_key

.do_shutdown:
    mov ax, 0x530e
    xor bx, bx
    mov cx, 0x0102
    int 0x15
    mov ax, 0x5307
    mov bx, 0x0001
    mov cx, 0x0003
    int 0x15

    ; fallback if APM is unsupported by BIOS
    cli
    hlt
    jmp $-2

	;call pause2secs
.do_boot:
	lgdt [gdt_descriptor]

	mov al, 'G'
    call send_serial

    mov ax, 0x0003
    int 0x10

	cli
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp CODE_SEL:pm_start

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

send_serial:
    push dx
    push ax
    mov ah, al
    mov dx, 0x3fd
.wait:
    in al, dx
    test al, 0x20
    jz .wait
    mov dx, 0x3f8
    mov al, ah
    out dx, al
    pop ax
    pop dx
    ret

read_dsksec1:
    mov si, dap
    mov ah, 0x42    ;extended (LBA) read - no CHS track/sector ceiling
	mov dl, [boot_drive]
	int 0x13
	jc .dsk_err
	ret

.dsk_err:
    mov si, dsk_err_msg
    call print_string
	cli
	hlt
	jmp $-2

dap:
    db 0x10     ;packet size (16 bytes)
    db 0    ;reserved, must be zero

dap_count:
    dw 120  ;sectors to read
    dw KERNEL ;dest offset
    dw 0 ;dest segment -> 0000:8000
    dq 1 ;starting LBS (sector right after the boot sector)

print_string:
	lodsb
	test al, al
	jz .done
	mov ah, 0x0e
	int 0x10
	jmp print_string

.done:
	ret

enable_a20:
	in al, 0x92
	or al, 0x02
    and al, 0xfe        ;mask out bit 0
	out 0x92, al
	ret

[bits 32]
pm_start:
	cli
	mov ebp, 0x130000
	mov ax, DATA_SEL
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, ebp

	mov al, 'P'
	call send_serial_32

	push dword MEMORY_MAP_BUFFER_PTR ;argument 2: mmap ptr address
	push dword [NUM_MEMORY_ENTRIES_PTR] ;argument 1: entries count value

	mov eax, KERNEL ;direct addressing mode
	jmp eax

send_serial_32:
    mov dx, 0x3fd
.waitt:
    in al, dx
    test al, 0x20
    jz .waitt
    mov dx, 0x3f8
    out dx, al
    ret

	cli
	hlt
	jmp $-2

gdt_start:
	gdt_null:
		;null descriptor, must always be zeroed
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

gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start

CODE_SEL equ 0x08
DATA_SEL equ 0x10

boot_drive db 0
msg db "UROS Bootloader, Loading Kernel...", 13, 10, 0
msg_prompt db "[B] to Boot or [S] to Shutdown: ", 0
dsk_err_msg db "Disk Error!", 0

times 510 - ($ - $$) db 0
dw 0xaa55

