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
    /*kmalloc does not zero memory, so initialise every field we rely on*/
    vfs_root->flags = VFS_DIR;/*root is a directory inode*/
    vfs_root->size = 0;
    vfs_root->ops = &nothfs_ops;
    /*root_start is volume-relative; NothFS converts it when reading*/
    vfs_root->fs_data = (void *)(uintptr_t)nothfs_root.superblock.root_start;
    return 0;
}

/*vfs_open: for now this only opens a name in the root directory*/
int
vfs_open(const char *name, struct vfs_file_t *file)
{
	if (!vfs_root || !vfs_root->ops || !vfs_root->ops->lookup || !name || !file)
		return -1;
	struct vfs_node_t *found = vfs_root->ops->lookup(vfs_root, name);/*ask NothFS to find the name*/
	if (!found)
		return -1;
	file->node = found;
	file->offset = 0;/*fresh file starts at offset 0*/
	return 0;
}

/*returns bytes read, 0 at EOF, -1 on error*/
int
vfs_read(struct vfs_file_t *file, void *buffer, uint64_t size)
{
	if (file->node->flags == VFS_DIR) return -1;
	if (!file || !file->node || !file->node->ops ||
	    !file->node->ops->read || !buffer)
		return -1;
	return file->node->ops->read(file, buffer, size);/*dispatch to the filesystem*/
}

void
vfs_close(struct vfs_file_t *file)
{
	if (!file || !file->node)
		return;
	kfree((void *)file->node);/*free the inode lookup created*/
	file->node = NULL;
	/*we'll worry about reference count later*/
}
