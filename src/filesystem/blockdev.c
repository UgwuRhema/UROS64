#include "blockdev.h"

#define MAX_BLOCK_DEVS 3
static struct block_device *devices[MAX_BLOCK_DEVS];/*too small?*/
static int number_of_devices = 0;

/*negative 1 on error*/
int
blockdev_register(struct block_device *dev)
{
	if (!dev || number_of_devices >= MAX_BLOCK_DEVS)
		return -1;
	devices[number_of_devices++] = dev;/*surely this is a valid implementation*/
	return 0;
}

struct block_device *
blockdev_get_primary(void)
{
	/*returns first device, apparently this is the primary drive*/
	if (number_of_devices == 0)
		return NULL;
	return devices[0];
}
