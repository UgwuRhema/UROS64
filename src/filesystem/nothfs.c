#include "nothfs.h"
#include "../func/func.h"
#include "../vga/vga.h"
#include "blockdev.h"
#include "ata.h"

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
	dev->read(dev, NOTHFS_VOLUME_LBA, 1, (uint8_t *)&sb);
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
	struct block_device *ata = primary_create();
	blockdev_register(ata);
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
