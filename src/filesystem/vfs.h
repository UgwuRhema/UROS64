#ifndef VFS_H
#define VFS_H

#include "nothfs.h"

struct vfs_ops_t
{
	int (*read)(struct vfs_file_t *, void *, uint64_t); /*read data from a file*/
	/*find a file by name and return a new node if found*/
	struct vfs_node_t *(*lookup)(struct vfs_file_t *, const char *);
};

/*generic inode*/
struct vfs_node_t
{
	uint16_t flags; /*file or directory*/
	uint64_t size;
	struct vfs_ops_t *ops;
	void *fs_data; /*nothfs will store it's data here*/
};

/*represent an open file*/
struct vfs_file_t
{
	struct vfs_node_t *node;/*what file are we reading?*/
	uint64_t offset;
} __attribute__((packed));

#endif
