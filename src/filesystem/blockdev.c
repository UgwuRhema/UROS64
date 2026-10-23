#include "blockdev.h"

#define MAX_BLOCK_DEVS 3
static struct block_device *devices[MAX_BLOCK_DEVS];/*too small?*/
static int number_of_devices = 0;

/*negative 1 on error*/
int
blockdev_register(struct block_device *dev)
{
	if (!dev)
		return -1;
	devices[number_of_devices++] = dev;/*surely this is a valid implementation*/
}

struct block_device *
blockdev_get_primary(void)
{
	/*returns forst device, apparently this is the primary drive*/
	return devices[0];
}
