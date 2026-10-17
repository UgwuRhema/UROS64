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

static uint8_t ata_drive_select = ATA_DRIVE_MASTER;

void
ata_read_sector(uint32_t lba, uint8_t *buffer)
{
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
		kp_log(" (DRV)ATA PIO timeout waiting for BSY. ", NOT_DONE);
		return;
	}

	/* check error bit */
	if (status & 0x01)
	{
		kp_log(" (DRV)Drive Rejected Read Command. ", NOT_DONE);
		return;
	}

	while (!(status & ATA_STATUS_DRQ))
		status = inb(ATA_PRIMARY_STATUS);

	/* pull 256 word (512 bytes) out of the data channel port */
	uint16_t *ptr = (uint16_t *)buffer;
	for (int i = 0; i < 256; ++i)
		ptr[i] = inw(ATA_PRIMARY_DATA);
}
