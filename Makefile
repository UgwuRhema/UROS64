AS := nasm
ASF := -f bin

BOOT_SRC := ./boot/boot.asm
BOOT := ./boot/uboot

IMG := uros.img

.PHONY: bloader
bloader: ./boot/boot.asm
	$(AS) $(ASF) -o $(BOOT) $(BOOT_SRC)

img:
	qemu-img create -f raw $(IMG) 30M

loadb:
	dd if=$(BOOT) of=$(IMG) bs=512 count=1 conv=notrunc

qemu:
	qemu-system-x86_64 \
		-boot menu=on \
		-drive format=raw,file=$(IMG),if=ide \
		-m 512 \
		-smp 1 \
		-enable-kvm \
		-machine q35 \
		-cpu Nehalem-v2 \
		-display gtk
