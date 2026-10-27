#ifndef NOTHFS_H
#define NOTHFS_H

#include <stdint.h>
#include "blockdev.h"

#define NOTHFS_MAGIC "NOTHFS\0"
#define NOTHFS_VOLUME_LBA 256

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
	uint8_t reserved[462];/*padding*/
} __attribute__((packed));

/*tracker*/
struct nothfs_mount_info {
	struct block_device *dev;
	uint64_t volume_start_lba;
	struct nothfs_superblock superblock;
};

extern struct nothfs_mount_info nothfs_root;
extern int nothfs_mount(struct block_device *);
void nothfs_test(void);

#endif
