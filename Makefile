AS := nasm
ASF := -f bin

BOOT_SRC := ./boot/boot.asm
BOOT := ./boot/uboot
S2_SRC := ./boot/stage2.asm
S2 := ./boot/uboot2
KE_SRC := ./src/entry.asm
KE := ./src/uroskrnl

IMG := uros.img

.PHONY: bloader stage2
bloader: ./boot/boot.asm
	$(AS) $(ASF) -o $(BOOT) $(BOOT_SRC)
	chmod +x $(BOOT)
	$(AS) $(ASF) -o $(S2) $(S2_SRC)
	chmod +x $(S2)

kernel:
	$(AS) $(ASF) -o $(KE) $(KE_SRC)
	chmod +x $(KE)

img:
	qemu-img create -f raw $(IMG) 10M

load:
	dd if=$(BOOT) of=$(IMG) bs=512 count=1 conv=notrunc
	dd if=$(S2) of=$(IMG) bs=512 seek=1 conv=notrunc
	dd if=$(KE) of=$(IMG) bs=512 seek=3 conv=notrunc

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
