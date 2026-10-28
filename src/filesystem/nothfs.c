#include "nothfs.h"
#include "../func/func.h"
#include "../vga/vga.h"
#include "blockdev.h"
#include "ata.h"
#include "../memory/heap.h"

struct nothfs_mount_info nothfs_root;

_Static_assert(sizeof(struct nothfs_superblock) <= 512,
               "nothfs superblock must fit in one sector");

int /*-1 on device error, 1 on signature error so we can distinguish*/
nothfs_mount(struct block_device *dev)
{
	if (!dev)
		return -1;
	struct nothfs_superblock sb;
	/*since the superblock was written to LBA256*/
	if (dev->read(dev, NOTHFS_VOLUME_LBA, 1, (uint8_t *)&sb) != 0)
		return -1;
	if (strcmp((char *)sb.signature, "NOTHFS") != 0)
		return 1;
	nothfs_root.dev = dev;
	nothfs_root.volume_start_lba = NOTHFS_VOLUME_LBA;
	nothfs_root.superblock = sb;
	return 0;/*success*/
}

void
nothfs_test(void)
{
	kp_log("(DRV)Drive Test", DONE);
	//struct block_device *ata = primary_create();
	//blockdev_register(ata);
	struct block_device *disk = blockdev_get_primary();
	if (!disk)
	{
		kp_log("NULL blockdevice", NOT_DONE);
		return;
	}
	int mnt = nothfs_mount(disk);
	if (mnt == 0) {
		kprint("Signature: ", PURPLE);
		kprint((char *)nothfs_root.superblock.signature, WHITE);
		kprint("\n", WHITE);
		kprint("Version: ", PURPLE);
		kprint_num((uint64_t)nothfs_root.superblock.version, GREEN); kprint("\n", WHITE);
		kprint("Unique-Id: ", PURPLE); kprint_hex(nothfs_root.superblock.unique_id, WHITE);
		kprint("\n", WHITE);
		kprint("Volume LBA: ", PURPLE);
		kprint_num(nothfs_root.superblock.root_start, GREEN); kprint("\n", WHITE);
		kprint("Sector Size: ", PURPLE);
		kprint_num((uint64_t)nothfs_root.superblock.sector_size, GREEN); kprint("\n", WHITE);
	} else if (mnt < 0){
		kp_log("NULL blockdevice", NOT_DONE);
		return;
	} else {
		kp_log("Disk is not formatted to NothFS, Signatures don't match", NOT_DONE);
		return;
	}
}

/*returns no of files/directories found or -1 on error*/
int
nothfs_readdir(void)
{
	struct nothfs_dirent *dirent;
	uint64_t current_lba = nothfs_root.volume_start_lba + nothfs_root.superblock.root_start;
	int file_count = 0;
	/*read the first sector of the root directory into  a 512byte buffer*/
	uint8_t *buffer = kmalloc(512);
	if (nothfs_root.dev->read(nothfs_root.dev, current_lba, 1, buffer) != 0){
		return -1;
	}
	dirent = (struct nothfs_dirent *)buffer;
	for (int i = 0; i < 8; ++i)
	{
		if (dirent[i].attr == 0)/*if attr is 0, it's an empty slot. EOD*/
			break;
		if (dirent[i].attr & NOTHFS_ATTR_DELETED)
			continue;
		/*just a debug*/
		kprint(" [FILE] ", WHITE);
		kprint((char *)dirent[i].name, GREEN);
		kprint(" (", WHITE);
		kprint_num(dirent[i].size, GREEN);
		kprint(" bytes)\n", WHITE);
		file_count++;
	}
	return file_count;
}
