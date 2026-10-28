#ifndef NOTHFS_H
#define NOTHFS_H

#include <stdint.h>
#include "blockdev.h"

#define NOTHFS_MAGIC "NOTHFS\0"
#define NOTHFS_VOLUME_LBA 256

#define NOTHFS_DIRENT_SIZE 64
#define NOTHFS_NAME_LEN 38

#define NOTHFS_ATTR_DIR (1 << 0)
#define NOTHFS_ATTR_CONTIG (1 << 1)
#define NOTHFS_ATTR_DELETED (1 << 2)

struct nothfs_dirent
{
	uint16_t attr;
	uint8_t name[NOTHFS_NAME_LEN];/*null terminated filename*/
	uint64_t clus;/*starting sector*/
	uint64_t size;
	uint64_t datetime;
	uint8_t reserved[8];/*padding*/
} __attribute__((packed));

/*we match the exact one used in mkfs.nothfs*/
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

/*tracker*/
struct nothfs_mount_info {
	struct block_device *dev;
	uint64_t volume_start_lba;
	struct nothfs_superblock superblock;
};

extern struct nothfs_mount_info nothfs_root;
extern int nothfs_mount(struct block_device *);
extern void nothfs_test(void);
extern int nothfs_readdir(void);

#endif
