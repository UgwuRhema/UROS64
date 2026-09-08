AS := nasm
ASF := -f bin

BOOT_SRC := ./boot/boot.asm
BOOT := ./boot/uboot

.PHONY: bloader
bloader: ./boot/boot.asm
	$(AS) $(ASF) -o $(BOOT) $(BOOT_SRC)

img:
	qemu-img create -f raw uros.img 5M

