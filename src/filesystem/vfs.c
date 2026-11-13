#include "vfs.h"
#include "../memory/heap.h"
#include "nothfs.h"

struct vfs_node_t *vfs_root = NULL;
/*VirtualFile System initialization function*/
int vfs_init(void)
{
    vfs_root = (struct vfs_node_t *)kmalloc(sizeof(struct vfs_node_t));
    if (!vfs_root)
        return 1;
    vfs_root->ops = &nothfs_ops;
    vfs_root->fs_data = (void *)(uintptr_t)nothfs_root.superblock.root_start;
    return 0;
}

/*vfs_open*/

/*returns bytes read and 1 on error*/
int
vfs_read(struct vfs_file_t *file, void *buffer, uint64_t size)
{
	if (!file || !buffer)
		return 1;
	return vfs_root->ops->read(file, buffer, size);
}
