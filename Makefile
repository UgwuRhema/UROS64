AS := nasm
ASF := -f bin

BOOT_SRC := ./boot/boot.asm
BOOT := ./boot/uboot

boot: ./boot/boot.asm
	$(AS) $(ASF) -o $(BOOT) $(BOOT_SRC)

