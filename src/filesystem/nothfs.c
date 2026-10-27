#include "nothfs.h"
#include "../func/func.h"
#include "../vga/vga.h"

struct nothfs_mount_info nothfs_root;

int /*-1 on error*/
nothfs_mount(struct block_device *dev)
{
	if (!dev)
		return -1;
	uint8_t buffer[512];
	/*since the superblock was written to LBA256*/
	dev->read(dev, NOTHFS_VOLUME_LBA, 1, buffer);
	struct nothfs_superblock *read_sblock = (struct nothfs_superblock *)buffer;
	if (strcmp((char *)read_sblock->signature, "NOTHFS") != 0)
		return -1;
	return 0;
}
