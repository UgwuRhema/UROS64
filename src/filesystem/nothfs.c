#include "nothfs.h"
#include "../func/func.h"
#include "../vga/vga.h"
#include "blockdev.h"
#include "ata.h"
#include "../memory/heap.h"
#include <stdint.h>
#include "vfs.h"

struct nothfs_mount_info_t nothfs_root;
struct vfs_ops_t nothfs_ops = {
	.lookup = &nothfs_lookup,
	.read = &nothfs_read
};

_Static_assert(sizeof(struct nothfs_superblock) <= 512,
               "nothfs superblock must fit in one sector");
_Static_assert(sizeof(struct nothfs_dirent_t) == NOTHFS_DIRENT_SIZE,
               "nothfs dirent size must match NOTHFS_DIRENT_SIZE");

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
	nothfs_root.volume_start_lba = NOTHFS_VOLUME_LBA;/*every NothFS address is relative to this*/
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
		kprint("Root Start: ", PURPLE);
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
	uint64_t current_lba = nothfs_root.volume_start_lba + nothfs_root.superblock.root_start;/*root_start is volume-relative*/
	int file_count = 0;
	/*read the 3 directory sectors*/
	uint8_t *buffer = kmalloc(NOTHFS_DIR_SECTORS * NOTHFS_SECTOR_SIZE);
	if (!buffer)
		return -1;
	if (nothfs_root.dev->read(nothfs_root.dev, current_lba, NOTHFS_DIR_SECTORS, buffer) != 0){
		kfree(buffer);
		return -1;
	}
	dirent = (struct nothfs_dirent_t *)buffer;
	/*scan every 64-byte dirent slot*/
	for (int i = 0; i < (int)((NOTHFS_DIR_SECTORS * NOTHFS_SECTOR_SIZE) / sizeof(struct nothfs_dirent_t)); ++i)
	{
		/*attr 0 means this slot is free not EOD, an empty
		 * slot can sit before a valid entry so we keep scanning instead of breaking out early like before*/
		if (dirent[i].attr == 0)
			continue;
		if (dirent[i].attr & NOTHFS_ATTR_DELETED)
			continue;
		/*just a debug*/
		kprint((char *)dirent[i].name, WHITE);
		kprint(" ", WHITE);/*space between*/
		kprint_num(dirent[i].size, BLUE); kprint("\n", WHITE);
		file_count++;
	}
	kfree(buffer);
	return file_count;
}

void
fake_nothfs_dirent_test(int add)
{
	/*one place decides where the fake file lives, so the dirent and the
	 *data can never disagree again*/
	const uint64_t test_clus = 30;/*volume-relative sector*/
	const char payload[] = "Hello World\n";
	uint8_t buffer[512];
	memset(buffer, 0, sizeof(buffer));
	struct nothfs_dirent_t *dummy = (struct nothfs_dirent_t *)buffer;
	uint32_t dirent_lba = (uint32_t)(nothfs_root.volume_start_lba + nothfs_root.superblock.root_start + add);
	uint64_t data_lba = nothfs_root.volume_start_lba + test_clus;/*where clus actually points on disk*/
	kprint_num((uint64_t)dirent_lba, WHITE);
	dummy->attr = NOTHFS_ATTR_CONTIG;
	kstrcpy((char *)dummy->name, "book.txt");
	dummy->clus = test_clus;
	/*size is a byte count, not a sector count, and a file does not
	 *include its terminating NUL*/
	dummy->size = sizeof(payload) - 1;
	/*write the card into the root directory*/
	int a = nothfs_root.dev->write(nothfs_root.dev, dirent_lba, 1, buffer);
	if (a != 0) {
		kp_log("Fake-nothfs_dirent_t_test_fail. ", NOT_DONE);
		return;
	}
	/*write the file data as well, so the test is self-contained*/
	memset(buffer, 0, sizeof(buffer));
	kstrcpy((char *)buffer, payload);
	a = nothfs_root.dev->write(nothfs_root.dev, data_lba, 1, buffer);
	if (a != 0)
		kp_log("Fake-nothfs_data_test_fail. ", NOT_DONE);
	/*we will now create anothre fake dirent*/
}

int
nothfs_read(struct vfs_file_t *file, void *buffer, uint64_t size)
{
    if (!file || !file->node || !buffer)
        return -1;
    /*the superblock is the authority; fall back to the format default if a
     *bad image has a zero here*/
    uint32_t sector_size = nothfs_root.superblock.sector_size;
    if (sector_size == 0)
        sector_size = NOTHFS_SECTOR_SIZE;
    uint8_t *temp = kmalloc(sector_size);
    if (!temp)
        return -1;
    uint64_t starting_clus = (uint64_t)(uintptr_t)file->node->fs_data;/*okay we get the startin cluster*/
    uint8_t *dest = (uint8_t *)buffer;/*advance this, not buffer itself*/
    uint64_t total_bytes_read = 0;
    while (total_bytes_read < size && file->offset < file->node->size)
    {
        uint64_t sector_index = file->offset / sector_size;
        uint64_t byte_offset = file->offset % sector_size;/*position inside sector*/
        uint64_t lba = nothfs_root.volume_start_lba + starting_clus + sector_index;/*volume sector -> disk LBA*/
        if (nothfs_root.dev->read(nothfs_root.dev, lba, 1, temp) != 0)
        {
            kfree(temp);
            return -1;
        }
        uint64_t bytes_left_in_sector = sector_size - byte_offset;/*rest of this sector*/
        uint64_t bytes_left_in_file = file->node->size - file->offset;/*rest of the file*/
        uint64_t chunk = size - total_bytes_read;/*rest of the caller's buffer*/
        /*copy no more than the request, the sector, or the rest of the file*/
        if (chunk > bytes_left_in_sector)
            chunk = bytes_left_in_sector;
        if (chunk > bytes_left_in_file)
            chunk = bytes_left_in_file;
        memcpy(dest, temp + byte_offset, chunk);
        dest += chunk;
        file->offset += chunk;
        total_bytes_read += chunk;
    }
    kfree(temp);
    return (int)total_bytes_read;
}

/*this is very simple*/
struct vfs_node_t *
nothfs_lookup(struct vfs_node_t *dir, const char *name)
{
	struct nothfs_dirent_t *dirent;
    uint64_t dir_start_clus = 0;
    if (!dir) dir_start_clus = nothfs_root.superblock.root_start;/*defaults to root directory*/
    else dir_start_clus = (uint64_t)(uintptr_t)dir->fs_data;
	uint64_t current_lba = nothfs_root.volume_start_lba + dir_start_clus;/*dir cluster -> disk LBA*/
	/*read the 3 directory sectors*/
	uint8_t *buffer = kmalloc(NOTHFS_DIR_SECTORS * NOTHFS_SECTOR_SIZE);
	if (!buffer) return NULL;
	if (nothfs_root.dev->read(nothfs_root.dev, current_lba, NOTHFS_DIR_SECTORS, buffer) != 0){
		kfree(buffer);
		return NULL;
	}
	dirent = (struct nothfs_dirent_t *)buffer;
	for (int i = 0; i < (int)((NOTHFS_DIR_SECTORS * NOTHFS_SECTOR_SIZE) / sizeof(struct nothfs_dirent_t)); ++i)
	{
		/*attr 0 is a free slot, not end of directory*/
		if (dirent[i].attr == 0)
			continue;
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
            memset(nnode, (uint8_t)0, (uint32_t)sizeof(*nnode));
			/*translate the on-disk dirent into a generic in-memory inode*/
			nnode->flags = (dirent[i].attr & NOTHFS_ATTR_DIR) ? VFS_DIR : VFS_FILE;
			nnode->size = dirent[i].size;
			nnode->ops = &nothfs_ops;
			/*volume-relative, not an absolute LBA; nothfs_read() converts it*/
			nnode->fs_data = (void *)(uintptr_t)dirent[i].clus;
			kfree(buffer);
			return nnode;
			/*but why is the vfs_node_t dir there as an argument, i dont see any use for it*/
		}
	}
	kfree(buffer);
	return NULL;
}
