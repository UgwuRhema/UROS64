#include "nothfs.h"
#include "../func/func.h"
#include "../vga/vga.h"
#include "blockdev.h"
#include "ata.h"
#include "../memory/heap.h"

struct nothfs_mount_info_t nothfs_root;
struct vfs_ops_t nothfs_ops = {
	.lookup = &nothfs_lookup,
	.read = &nothfs_read
};

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
	struct nothfs_dirent_t *dirent;
	uint64_t current_lba = nothfs_root.volume_start_lba + nothfs_root.superblock.root_start;
	int file_count = 0;
	/*read the first sector of the root directory into  a 512byte buffer*/
	uint8_t *buffer = kmalloc(512);
	if (nothfs_root.dev->read(nothfs_root.dev, current_lba, 1, buffer) != 0){
		kfree(buffer);
		return -1;
	}
	dirent = (struct nothfs_dirent_t *)buffer;
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
	kfree(buffer);
	return file_count;
}

void
fake_nothfs_dirent_test(void)
{
	uint8_t buffer[512];
	memset(buffer, 0, 512);
	struct nothfs_dirent_t *dummy = (struct nothfs_dirent_t *)buffer;
	uint32_t current_lba = (uint32_t)(nothfs_root.volume_start_lba + nothfs_root.superblock.root_start);/*262*/
	kprint_num((uint64_t)current_lba, WHITE);
	dummy->attr = NOTHFS_ATTR_CONTIG;
	kstrcpy((char *)dummy->name, "hello.txt");
	dummy->clus = 10;
	dummy->size = 1024;
	int a = nothfs_root.dev->write(nothfs_root.dev, current_lba, 1, (uint8_t *)dummy);
	if (a != 0)
		kp_log("Fake-nothfs_dirent_t_test_fail. ", NOT_DONE);
	/*we will now create anothre fake dirent*/
}

int
nothfs_read(struct vfs_file_t *file, void *buffer, uint64_t size)
{
	if (!file || !buffer) return -1;
	uint64_t starting_clus = (uint64_t)(uintptr_t)file->node->fs_data; /*we get the starting cluster that we saved*/
	uint64_t lba = nothfs_root.volume_start_lba + starting_clus;	
	if (nothfs_root.dev->read(nothfs_root.dev, lba, 1, buffer) != 0)
		return -1;
	return 0;
}

/*this is very simple*/
struct vfs_node_t *
nothfs_lookup(struct vfs_node_t *dir, const char *name)
{
	struct nothfs_dirent_t *dirent;
	uint64_t current_lba = nothfs_root.volume_start_lba + nothfs_root.superblock.root_start;
	/*read the first sector of the root directory into  a 512byte buffer*/
	uint8_t *buffer = kmalloc(512);
	if (!buffer) return NULL;
	if (nothfs_root.dev->read(nothfs_root.dev, current_lba, 1, buffer) != 0){
		kfree(buffer);
		return NULL;
	}
	dirent = (struct nothfs_dirent_t *)buffer;
	for (int i = 0; i < 8; ++i)
	{
		if (dirent[i].attr == 0)/*if attr is 0, it's an empty slot. EOD*/
			break;
		if (dirent[i].attr & NOTHFS_ATTR_DELETED)
			continue;
		if(strcmp((char *)dirent[i].name, name) == 0)
		{
			/*we now make a new vfs_node_t*/
			struct vfs_node_t *nnode = (struct vfs_node_t *)kmalloc(sizeof(struct vfs_node_t));
			if (!nnode) {
				kfree(buffer);
				return NULL;
			}
            /*fixed positioning, although we can mall a 0 page
             * we still check for allocation failure before any operations*/
            memset(nnode, 0, sizeof(*nnode));
			nnode->size = dirent[i].size;
			nnode->ops = &nothfs_ops;
			nnode->fs_data = (void *)(uintptr_t)dirent[i].clus;/*we save starting NothFS sector*/
			kfree(buffer);
			return nnode;
			/*but why is the vfs_node_t dir there as an argument, i dont see any use for it*/
		}
	}
	kfree(buffer);
	return NULL;
}
