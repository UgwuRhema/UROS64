#include "ata.h"

static void 
ata_io_delay(void)
{
	/* a 400ns delay for the CPU to read the status regitser */
	inb(0x3f6);
	inb(0x3f6);
	inb(0x3f6);
	inb(0x3f6); /* 100ns each */
}
