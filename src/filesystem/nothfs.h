#ifndef NOTHFS_H
#define NOTHFS_H

#include <stdint.h>
#include "blockdev.h"

#define NOTHFS_MAGIC "NOTHFS\0" /*magic the superblock*/
#define NOTHFS_VOLUME_LBA 256 /*nothfs
superblock defailt logical block address*/

#define NOTHFS_DIRENT_SIZE 64 /*default dirent size*/
#define NOTHFS_NAME_LEN 38 /*max filename len*/

/*each sector size*/
#define NOTHFS_SECTOR_SIZE 512
/*number of sectors we scan at the start of a directory*/
#define NOTHFS_DIR_SECTORS 3

#define NOTHFS_ATTR_DIR (1 << 0)
#define NOTHFS_ATTR_CONTIG (1 << 1)
#define NOTHFS_ATTR_DELETED (1 << 2)

/*nothfs_dirent_t struct to represent a nothfs directory entry
 * which are either files or directories*/
struct nothfs_dirent_t
{
	uint16_t attr;/*FILE, DIR or DELETED*/
	uint8_t name[NOTHFS_NAME_LEN];/*null terminated filename*/
	uint64_t clus;/*starting sector, relative to volume start*/
	uint64_t size;/*file size in bytes*/
	uint64_t datetime;
	/*already packed*/
} __attribute__((packed));

/*we match the exact one used in mkfs.nothfs*/
/*single nothfs_superblock*/
struct nothfs_superblock {
	uint8_t signature[8];
	uint32_t version;
	uint32_t sector_size;
	uint64_t total_sectors;
	uint64_t bitmap_start;/*rel to volume start*/
	uint64_t bitmap_sectors;
	uint64_t root_start;/*rel to volume start*/
	uint64_t unique_id;
	uint8_t reserved[512 - 56];/*padding*/
} __attribute__((packed));

/*tracker
 * we use this to track every bit of the nothfs disk
 * after it has been mounted*/
struct nothfs_mount_info_t {
	struct block_device *dev;
	uint64_t volume_start_lba;
	struct nothfs_superblock superblock;
};

/*important pre-definitions*/
struct vfs_file_t;
struct vfs_node_t;
struct vfs_ops_t;

/*nothfs functions*/
extern struct nothfs_mount_info_t nothfs_root;

extern struct vfs_ops_t nothfs_ops;

/*mount the block device*/
extern int nothfs_mount(struct block_device *);

extern void nothfs_test(void);

extern int nothfs_readdir(struct vfs_node_t *);/*less*/
extern int nothfs_readdir2(struct vfs_node_t *);/*more verbose*/

extern void fake_nothfs_dirent_test(int);/*test*/
extern int nothfs_read(struct vfs_file_t *, void *, uint64_t);
extern struct vfs_node_t *nothfs_lookup(struct vfs_node_t *, const char *);

#endif
