#include "ata.h"
#include <stdint.h>

static void 
ata_io_delay(void)
{
	/* a 400ns delay for the CPU to read the status regitser */
	inb(0x3f6);
	inb(0x3f6);
	inb(0x3f6);
	inb(0x3f6); /* 100ns each */
}

static uint8_t ata_drive_select = ATA_DRIVE_MASTER;

int
ata_read_sector(uint32_t lba, uint8_t *buffer)
{
    if (!buffer) return -1;
	/* send bits 24-27 of lba along with drive identifier */
	outb(ATA_PRIMARY_DRIVE, ata_drive_select | ((lba >> 24) & 0x0f));
	outb(ATA_PRIMARY_SECCOUNT, 1); /* 1 sector at a time.. */
	/* send the remaining LBA bytes sequentially accross ports */
	outb(ATA_PRIMARY_LBA_LOW, (uint8_t)lba);
	outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
	outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)(lba >> 16));
	/* fire the read scetors command register byte */
	outb(ATA_PRIMARY_COMMAND, 0x20);
	ata_io_delay();
	/* poll the status register */
	uint32_t timeout = 1000000;
	uint8_t status = inb(ATA_PRIMARY_STATUS);
	while ((status & ATA_STATUS_BSY) && --timeout)
		status = inb(ATA_PRIMARY_STATUS);
	if (!timeout)
	{
		kp_log("(DRV)ATA PIO timeout waiting for BSY. ", NOT_DONE);
		return -1;
	}
	/* check error bit */
	if (status & 0x01)
	{
		kp_log("(DRV)Drive Rejected Read Command. ", NOT_DONE);
		return -1;
	}
	uint32_t timeout2 = 1000000;
	while ((!(status & ATA_STATUS_DRQ)) && --timeout2)
		status = inb(ATA_PRIMARY_STATUS);
	if (!timeout2)
	{
		kp_log("(DRV)ATA PIO timeout waiting for !DRQ. ", NOT_DONE);
		return -1;
	}
	/* pull 256 word (512 bytes) out of the data channel port */
	uint16_t *ptr = (uint16_t *)buffer;
	for (int i = 0; i < 256; ++i)
		ptr[i] = inw(ATA_PRIMARY_DATA);
	return 0; /* success! */
}

int
ata_write_sector(uint32_t lba, uint8_t *buffer)
{
   if (!buffer)
        return -1;
    uint32_t timeout = 1000000;
    uint8_t status = inb(ATA_PRIMARY_STATUS);
    while ((status & ATA_STATUS_BSY) && --timeout); /* wait for drive to not be busy */
    if (!timeout)
        return 1;
    outb(ATA_PRIMARY_DRIVE, ata_drive_select | ((lba >> 24) & 0x0f));
    outb(ATA_PRIMARY_LBA_LOW, (uint8_t)lba); /* send the remaining LBA bytes */
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_PIO); /* issue the ATA PIO write command */
    uint32_t timeout2 = 1000000;
    while (!(inb(ATA_PRIMARY_STATUS) & ATA_STATUS_DRQ) && --timeout2);
    if (!timeout2)
        return 1;
    uint16_t *ptr = (uint16_t *)buffer;
    for (int i = 0; i < 256; ++i)
        outw(ATA_PRIMARY_DATA, ptr[i]);
    #define CACHE_FLUSH 0xe7
    outb(ATA_PRIMARY_COMMAND, CACHE_FLUSH); /*flush the cache*/
    while ((inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY));
    return 0; /*success*/
}

/*returns 1 if a drive is successfully detected and 0 is not or errored*/
int
ata_identify_slave(void)
{
	outb(ATA_PRIMARY_DRIVE, ATA_DRIVE_SLAVE);/*select the primary slave drive*/
	outb(ATA_PRIMARY_SECCOUNT, 0x00);
	outb(ATA_PRIMARY_LBA_LOW, 0x00);
	outb(ATA_PRIMARY_LBA_MID, 0x00);
	outb(ATA_PRIMARY_LBA_HIGH, 0x00);
	outb(ATA_PRIMARY_COMMAND, 0xec);/*identify command*/
	uint8_t status = inb(ATA_PRIMARY_STATUS);
	if (status == 0)
		return 0;/*drive doesn't exist*/
	while((status & ATA_STATUS_BSY) && !(status ATA_STATUS_DRQ))/*kind of same thing we did in ata_read and ata_write*/
		status = inb(ATA_PRIMARY_STATUS);
	if (status & 0x01)
		return 0;/*drive flagged and error*/
	uint16_t info[256];
	for (int i = 0; i < 256; ++i)/*pull the 512 byte metadata*/
		info[i] = inw(ATA_PRIMARY_DATA);
	kp_log("(DRV)External Hard-disk detected. \n", ONLINE);
	// Drive model description string lives across words 27 to 46
	for (int i = 27; i <= 46; ++i)
	{
		char high = (char)(info[i] >> 8);  // Upper byte
		char low  = (char)(info[i] & 0xFF); // Lower byte

		// ATA text standard streams characters byte-swapped
		if (high != ' ') kprint_c(high, GREEN);
		if (low != ' ')  kprint_c(low), GREEN;
	}
	// Extract total 28-bit LBA sectors out of words 60 and 61
	u32 total_sectors = ((u32)info[61] << 16) | info[60];
	// stash it in the global
	kprint(" (", WHITE);
	kprint_num((total_sectors * 512) / (1024 * 1024), GREEN);
	kprint(" MB)\n", WHITE);
	return 1; // Drive successfully loaded!
}
