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
int
vfs_open(const char *dir_name, struct vfs_file_t *file)
{
	struct vfs_node_t *found = vfs_root->ops->lookup(vfs_root, dir_name);
	if (found)
	{
		file->node = found;
		file->offset = 0;
		return 0;
	}
	return 1; /*on failure*/
}

/*returns bytes read and 1 on error*/
int
vfs_read(struct vfs_file_t *file, void *buffer, uint64_t size)
{
	if (!file || !buffer)
		return 1;
	return file->node->ops->read(file, buffer, size);
}

void
vfs_close(struct vfs_file_t *file)
{
	kfree((void *)file->node);
	/*we'll worry about reference count later*/
}
