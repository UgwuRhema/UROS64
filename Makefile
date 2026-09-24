# unrestricted means it yours
# figure out the building yourself
AS := nasm
ASF := -f bin
ASF2 := -f elf64
CC := cc
CFLAGS := -ffreestanding -c -fno-pie -fno-stack-protector -mgeneral-regs-only -nostdlib -fno-builtin -Wall -Wextra -O2 -march=x86-64 -mno-red-zone
LD := /usr/bin/ld
LDFLAGS := -nostdlib -static
LINK := ./src/linker.ld

BOOT_SRC := ./boot/boot.asm
BOOT := ./boot/uboot
S2_SRC := ./boot/stage2.asm
S2 := ./boot/uboot2
KE_SRC := ./src/kernel/entry.asm
KE_O := ./src/kernel/entry.o
K := ./src/kernel/uroskrnl.c
K_O := ./src/kernel/uroskrnl.o
KER := ./src/kernel/uroskrnl.elf
KERN := ./src/kernel/uroskrnl
VGA := ./src/vga/vga.c
VGA_O := ./src/vga/vga.o
FUNC := ./src/func/func.c
FUNC_O := ./src/func/func.o
INT := ./src/ints/interrupts.c
INT_O := ./src/ints/interrupts.o
HAND := ./src/ints/handlers.c
HAND_O := ./src/ints/handlers.o
APIC := ./src/ints/apic.c
APIC_O := ./src/ints/apic.o
SHELL_SRC := ./src/shell/shell.c
SHELL_O := ./src/shell/shell.o


IMG := uros.img

.PHONY: bloader
bloader: ./boot/boot.asm
	$(AS) $(ASF) -o $(BOOT) $(BOOT_SRC)
	chmod +x $(BOOT)
	$(AS) $(ASF) -o $(S2) $(S2_SRC)
	truncate -s 1024 $(S2)
	chmod +x $(S2)

kernel:
	$(AS) $(ASF2) -o $(KE_O) $(KE_SRC)
	$(CC) $(CFLAGS)	-o $(K_O) $(K)
	$(CC) $(CFLAGS) -o $(VGA_O) $(VGA)
	$(CC) $(CFLAGS) -o $(FUNC_O) $(FUNC)
	$(CC) $(CFLAGS) -o $(HAND_O) $(HAND)
	$(CC) $(CFLAGS) -o $(INT_O) $(INT)
	$(CC) $(CFLAGS) -o $(APIC_O) $(APIC)
	$(CC) $(CFLAGS) -o $(SHELL_O) $(SHELL_SRC)
	$(LD) $(LDFLAGS) -T $(LINK) -o $(KER) $(KE_O) $(K_O) $(VGA_O) $(FUNC_O) $(INT_O) $(HAND_O) \
	$(APIC_O) $(SHELL_O)
	objcopy -O binary $(KER) $(KERN)
	rm -rf $(KER)
img:
	qemu-img create -f raw $(IMG) 10M
iso:
	mkdir -p ./iso_root
	cp $(IMG) ./iso_root
	xorriso -as mkisofs \
		-no-emul-boot \
		-b uros.img \
		-boot-load-size 10 \
		-o uros.ISO \
		./iso_root
	rm -rf iso_root

# i added this line cause i wanted to commit hehe

load:
	dd if=$(BOOT) of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=$(S2) of=$(IMG) bs=512 seek=1 conv=notrunc
	dd if=$(KERN) of=$(IMG) bs=512 seek=3 conv=notrunc

max:
	$(MAKE) kernel
	$(MAKE) load
	$(MAKE) qemu

qemu:
	qemu-system-x86_64 \
		-drive format=raw,file=$(IMG),if=ide \
		-m 1G \
		-smp 2 \
		-enable-kvm \
		-cpu host \
		-machine pc \
		-display gtk

gdb:
	qemu-system-x86_64 \
		-s \
		-S \
		-hda $(IMG) \
		-m 512 \
		-d int,cpu_reset \
		-no-reboot \
		-display gtk

