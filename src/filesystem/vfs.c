#include "vfs.h"
#include "../memory/heap.h"
#include "nothfs.h"

struct vfs_node_t *vfs_root;
int vfs_init(void)
{
    vfs_root = (struct vfs_node_t *)kmalloc(sizeof(struct vfs_node_t));
    if (!vfs_root)
        return 1;
    vfs_root->ops = &nothfs_ops;
    vfs_root->fs_data = (void *)(uintptr_t)nothfs_root.superblock.root_start;
    return 0;
}
