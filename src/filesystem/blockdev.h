#ifndef BLOCKDEV_H
#define BLOCKDEV_H

#include <stdint.h>
#include <stddef.h>

struct block_device
{
	const char *name;
	uint64_t sector_count;
	uint32_t sector_size;
	int(*read)(struct block_device *, uint64_t, uint32_t, void *);
	int (*write)(struct block_device *, uint64_t, uint32_t, const void *);
	void *driver_data;
};

/*register a new block device*/
extern int blockdev_register(struct block_device *);
/*get external primary block device for mounting*/
extern struct block_device *blockdev_get_primary(void);

#endif
