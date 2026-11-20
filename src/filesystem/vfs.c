#include "vfs.h"
#include "../memory/heap.h"
#include "nothfs.h"

struct vfs_node_t *vfs_root = NULL;
/*VirtualFile System initialization function*/
int vfs_init(void)
{
    vfs_root = (struct vfs_node_t *)kernel_heap_malloc(sizeof(struct vfs_node_t));
    if (!vfs_root)
        return 1;
    /*kernel_heap_malloc does not zero memory, so initialise every field we rely on*/
    vfs_root->flags = VFS_DIR;/*root is a directory inode*/
    vfs_root->size = 0;
    vfs_root->ops = &nothfs_ops;
    /*root_start is volume-relative; NothFS converts it when reading*/
    vfs_root->fs_data = (void *)(uintptr_t)nothfs_root.superblock.root_start;
    return 0;
}

/*vfs_open: for now this only opens a name in the root directory*/
int
vfs_open(const char *path, struct vfs_file_t *file)
{
	if (!vfs_root || !vfs_root->ops || !vfs_root->ops->lookup || !path || !file)
		return -1;
	struct vfs_node_t *found = vfs_root;
	const char *ptr = path;
	while (*ptr)
	{
		if (*ptr == '/'){
			ptr++;
			continue;
		}
		/*find the end of this component*/
		const char *end = ptr;
		while (*end && *end != '/')
		{
			end++;
		}
		char component[38];/*max filename size*/
		int len = end - ptr;
		memcpy(component, ptr, len);/*copy the current component string*/
		component[len] = '\0';/*null terminate the string*/
		found = found->ops->lookup(found, component);/*look the current component up*/
		if (!found)
		{
			return -1;/*doesn't exist*/
		}
		ptr = end;
	}
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
	kernel_heap_free((void *)file->node);/*free the inode lookup created*/
	file->node = NULL;
	/*we'll worry about reference count later*/
}
