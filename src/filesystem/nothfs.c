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
	uint8_t *buffer = kernel_heap_malloc(NOTHFS_DIR_SECTORS * NOTHFS_SECTOR_SIZE);
	if (!buffer)
		return -1;
	if (nothfs_root.dev->read(nothfs_root.dev, current_lba, NOTHFS_DIR_SECTORS, buffer) != 0){
		kernel_heap_free(buffer);
		return -1;
	}
	dirent = (struct nothfs_dirent_t *)buffer;
	/*scan every 64-byte dirent slot*/
	for (int i = 0; i < (int)((NOTHFS_DIR_SECTORS * NOTHFS_SECTOR_SIZE) / sizeof(struct nothfs_dirent_t)); ++i)
	{
		/*attr 0 means this slot is free not EOD, an empty
		 * slot can sit before a valid entry so we keep scanning instead of breaking out early like before*/
		 /* so raw file data with attr!=0 becomes a "file"*/
		if (dirent[i].attr == 0)
			continue;
		if (dirent[i].attr & NOTHFS_ATTR_DELETED)
			continue;
		if (dirent[i].attr & NOTHFS_ATTR_DIR)
		{
			/*every directory is 2 sectors?*/
			kprint((char *)dirent[i].name, BLUE);
			kprint(" ", WHITE);
			file_count++; /*a directory is still a file*/
		}
		if (dirent[i].attr & NOTHFS_ATTR_CONTIG) {
			/*just a debug*/
			if (dirent[i].attr & NOTHFS_ATTR_DIR) /*if we find a dir because dir's also have
			NOTHFS_ATTR_CONTIG */
				continue;
			kprint((char *)dirent[i].name, WHITE);
			kprint(" ", WHITE);/*space between*/
			file_count++;
		} else {
			kprint("Not a File\n", PURPLE);
		}
	}
	kernel_heap_free(buffer);
	return file_count;
}

int
nothfs_readdir2(void)
{
	struct nothfs_dirent_t *dirent;
	uint64_t current_lba = nothfs_root.volume_start_lba + nothfs_root.superblock.root_start;/*root_start is volume-relative*/
	int file_count = 0;
	/*read the 3 directory sectors*/
	uint8_t *buffer = kernel_heap_malloc(NOTHFS_DIR_SECTORS * NOTHFS_SECTOR_SIZE);
	if (!buffer)/*malloc failure*/
		return -1;
	if (nothfs_root.dev->read(nothfs_root.dev, current_lba, NOTHFS_DIR_SECTORS, buffer) != 0){
		/*read failure*/
		kernel_heap_free(buffer);
		return -1;
	}
	dirent = (struct nothfs_dirent_t *)buffer;
	/*scan every 64-byte dirent slot*/
	for (int i = 0; i < (int)((NOTHFS_DIR_SECTORS * NOTHFS_SECTOR_SIZE) / sizeof(struct nothfs_dirent_t)); ++i)
	{
		/*attr 0 means this slot is free not EOD, an empty
		 * slot can sit before a valid entry so we keep scanning instead of breaking out early like before*/
		 /* so raw file data with attr!=0 becomes a "file"*/
		if (dirent[i].attr == 0)
			continue;
		if (dirent[i].attr & NOTHFS_ATTR_DELETED)
			continue;
		if (dirent[i].attr & NOTHFS_ATTR_DIR)
		{
			/*every directory is 2 sectors?*/
			kprint("ATTR_DIR", WHITE);/*file-type attribute*/
			kprint(" - ", WHITE);
			kprint((char *)dirent[i].name, BLUE);
			kprint(" - ", WHITE); kprint_num(dirent[i].size, WHITE);
			kprint(" - ", WHITE);
			kprint_num(dirent[i].datetime,
						WHITE);
			kprint("\n", WHITE);
			file_count++;
		}
		if (dirent[i].attr & NOTHFS_ATTR_CONTIG) {
			/*just a debug*/
			if (dirent[i].attr & NOTHFS_ATTR_DIR) /*if we find a dir because dir's also have
			NOTHFS_ATTR_CONTIG */
				continue;
			kprint("ATTR_CONTIG", WHITE); /*same thing*/
			kprint(" - ", WHITE);
			kprint((char *)dirent[i].name, WHITE);
			kprint(" - ", WHITE); kprint_num(dirent[i].size, WHITE);
			kprint(" - ", WHITE); kprint_num((uint64_t)dirent[i].datetime, WHITE);
			kprint("\n", WHITE);
		} else {
			kprint("Not a File\n", PURPLE);
		}
	}
	kernel_heap_free(buffer);
	return file_count;
}

void
fake_nothfs_dirent_test(int add)
{
	/*this creates a nothfs_dirent with attribute = NOTHFS_CONTIG for representing
	 * a normal file */
	/*one place decides where the fake file lives, so the dirent and the
	 *data can never disagree again*/
	const uint64_t test_clus = 10;/*volume-relative sector*/
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
	dummy->datetime = 214822112026;
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
	/*this new nothfs_dirent will have attribute NOTHFS_DIR | NOTHFS_CONTIG to represent
	 * a directory */
	const uint64_t test_clus2 = 20;
	uint8_t another_buffer[512];
	memset(another_buffer, 0, sizeof(another_buffer));/*cleanup*/
	struct nothfs_dirent_t *dummy_dir = (struct nothfs_dirent_t *)another_buffer;
	uint32_t dirent2_lba = (uint32_t)(nothfs_root.volume_start_lba + nothfs_root.superblock.root_start + 1 + add);/*the next sector after
	the root dir, LBA 263*/
	uint64_t data_lba2 = nothfs_root.volume_start_lba + test_clus2;/*LBA 276*/
	kprint(" ", WHITE);kprint_num((uint64_t)dirent2_lba, WHITE);
	dummy_dir->attr = NOTHFS_ATTR_DIR | NOTHFS_ATTR_CONTIG;
	kstrcpy((char *)dummy_dir->name, "docs");
	dummy_dir->clus = test_clus2;
	dummy_dir->size = 1024; /*size for directories are different, every directory apart from root dir is 2 sectors*/
	dummy_dir->datetime = 214922112026;/*21:49 22|11|2026*/
	int b = nothfs_root.dev->write(nothfs_root.dev, dirent2_lba, 1, another_buffer);
	if (b != 0)
	{
		kp_log("Fake-nothfs_dirent2_test failure. ", NOT_DONE);
		return;
	}
	/*has no data yet*/
	/*let's crate it's data now, a new nothfs_dirent file*/
	const uint64_t test_clus3 = 30;/*the new file's data cluster*/
	char payload2[] = "I have divine intellect! God gave me divin intellect\n";
	uint8_t buffer3[512];
	memset(buffer3, 0, sizeof(buffer3));/*clean*/
	struct nothfs_dirent_t *file = (struct nothfs_dirent_t)buffer3;
	/*it is going to be placed in the clus sector of the "docs" directory entry*/
	uint32_t file_lba = (uint32_t)(data_lba2);/*right? I know this is a hardcode but it's a test*/
	/*for some reason i feel like this should be relative to it's underlying dir
	 * that is in this case docs clus...that file data should be at dirent2_lba + test_clus3 */
	uint64_t file_data = nothfs_root.volume_start_lba + test_clus3;
	kprint("", WHITE); kprint_num((uint64_t)file_lba, WHITE);
	file->attr = NOTHFS_ATTR_CONTIG;
	kstrcpy((char *)file->name, "docs_file.txt");
	file->clus = test_clus3;
	file->size = sizeof(payload2) - 1;
	file->size = 122923112026;
	if (nothfs_root.dev->write(nothfs_root.dev, file_lba, 1, buffer3) != 0)
	{
		kp_log("NOthfs_test write failure! ", NOT_DONE);
		return;
	}
	/*let's write the data while we can*/
	memset(buffer3, 0, sizeof(buffer3));
	kstrcpy((char *)buffer3, payload2);
	if (nothfs_root.dev->write(nothfs_root.dev, file_data, 1, buffer3) != 0)
	{
		kp_log("Failed to write file data to specifide cluster\n", NOT_DONE);
		return;
	}
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
    uint8_t *temp = kernel_heap_malloc(sector_size);
    if (!temp)
        return -1;
    uint64_t starting_clus = (uint64_t)(uintptr_t)file->node->fs_data;/*okay we get the startin cluster*/
    uint8_t *dest = (uint8_t *)buffer;/*advance this, not buffer itself*/
    uint64_t total_bytes_read = 0;
    while (total_bytes_read < size && 
			file->offset < file->node->size)
    {
        uint64_t sector_index = file->offset / sector_size;/*sector index*/
        uint64_t byte_offset = file->offset % sector_size;/*position inside sector*/
        uint64_t lba = nothfs_root.volume_start_lba + starting_clus + sector_index;/*volume sector -> disk LBA*/
        if (nothfs_root.dev->read(nothfs_root.dev, lba, 1, temp) != 0)
        {
            kernel_heap_free(temp);
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
	/*cleanup*/
    kernel_heap_free(temp);
    return (int)total_bytes_read;
}

/*this is very simple*/
struct vfs_node_t *
nothfs_lookup(struct vfs_node_t *dir, const char *name)
{
    /*we will have tobe able to parse aboslute paths and even relative paths*/
	struct nothfs_dirent_t *dirent;
    uint64_t dir_start_clus = 0;
    if (!dir) dir_start_clus = nothfs_root.superblock.root_start;/*defaults to root directory*/
    else dir_start_clus = (uint64_t)(uintptr_t)dir->fs_data;
	uint64_t current_lba = nothfs_root.volume_start_lba + dir_start_clus;/*dir cluster -> disk LBA*/
	/*read the 3 directory sectors*/
	uint8_t *buffer = kernel_heap_malloc(NOTHFS_DIR_SECTORS * NOTHFS_SECTOR_SIZE);
	if (!buffer) return NULL;
	if (nothfs_root.dev->read(nothfs_root.dev, current_lba, NOTHFS_DIR_SECTORS, buffer) != 0){
		kernel_heap_free(buffer);
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
		/*match name*/
        if(strncmp((char *)dirent[i].name, name, NOTHFS_NAME_LEN) == 0)
        {
            /*we now make a new vfs_node_t*/
            struct vfs_node_t *nnode = (struct vfs_node_t *)kernel_heap_malloc(sizeof(struct vfs_node_t));
            if (!nnode) {
                kernel_heap_free(buffer);
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
            kernel_heap_free(buffer);
            return nnode;
        }
	}
	kernel_heap_free(buffer);
	return NULL;
}
