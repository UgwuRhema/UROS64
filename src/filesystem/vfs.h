#ifndef VFS_H
#define VFS_H

#include "nothfs.h"

/*represent an open file*/
struct vfs_file
{
	struct nothfs_mount_info *info;
	uint64_t clus;
	uint64_t total_size;
	uint64_t rw_offset;
	uint8_t filename[38];
} __attribute__((packed));

#endif
