AS := nasm
ASF := -f bin
ASF2 := -f elf64
CC := cc
CFLAGS := -ffreestanding -c -nostdlib -fno-builtin -Wall -Wextra -O2 -march=x86-64 -mno-red-zone

BOOT_SRC := ./boot/boot.asm
BOOT := ./boot/uboot
S2_SRC := ./boot/stage2.asm
S2 := ./boot/uboot2
KE_SRC := ./src/kernel/entry.asm
KE_O := ./src/kernel/entry.o
K := ./src/kernel/kmain.c
k_O := ./src/kernel/kmain.o
KERN := ./src/kernel/uroskrnl

LINK := ./src/linker.ld

IMG := uros.img

.PHONY: bloader stage2
bloader: ./boot/boot.asm
	$(AS) $(ASF) -o $(BOOT) $(BOOT_SRC)
	chmod +x $(BOOT)
	$(AS) $(ASF) -o $(S2) $(S2_SRC)
	chmod +x $(S2)

kernel:
	$(AS) $(ASF2) -o $(KE_O) $(KE_SRC)
	

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
