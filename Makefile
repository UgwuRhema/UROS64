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
K := ./src/kernel/kmain.c
K_O := ./src/kernel/kmain.o
KER := ./src/kernel/uroskrnl.elf
KERN := ./src/kernel/uroskrnl

IMG := uros.img

.PHONY: bloader
bloader: ./boot/boot.asm
	$(AS) $(ASF) -o $(BOOT) $(BOOT_SRC)
	chmod +x $(BOOT)
	$(AS) $(ASF) -o $(S2) $(S2_SRC)
	chmod +x $(S2)

kernel:
	$(AS) $(ASF2) -o $(KE_O) $(KE_SRC)
	$(CC) $(CFLAGS)	-o $(K_O) $(K)
	$(CC) $(CFLAGS) -o ./src/vga/vga.o ./src/vga/vga.c
	$(LD) $(LDFLAGS) -T $(LINK) -o $(KER) $(KE_O) $(K_O)
	objcopy -O binary $(KER) $(KERN)
	rm -rf $(KER)
img:
	qemu-img create -f raw $(IMG) 10M

load:
	dd if=$(BOOT) of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=$(S2) of=$(IMG) bs=512 seek=1 conv=notrunc
	dd if=$(KERN) of=$(IMG) bs=512 seek=3 conv=notrunc

qemu:
	qemu-system-x86_64 \
		-boot menu=on \
		-drive format=raw,file=$(IMG),if=ide \
		-m 512 \
		-smp 1 \
		-enable-kvm \
		-cpu host \
		-machine pc \
		-display gtk
