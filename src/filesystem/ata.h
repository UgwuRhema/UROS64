#include "../func/func.h"
#include "../vga/vga.h"
#include <stdint.h>

#define ATA_PRIMARY_DATA 0x1f0
#define ATA_PRIMARY_ERR 0x1f1
#define ATA_PRIMARY_SECCOUNT 0x1f2
#define ATA_PRIMARY_LBA_LOW 0x1f3
#define ATA_PRIMARY_LBA_MID 0x1f4
#define ATA_PRIMARY_LBA_HIGH 0x1f5
#define ATA_PRIMARY_DRIVE 0x1f6
#define ATA_PRIMARY_STATUS 0x1f7
#define ATA_PRIMARY_COMMAND 0x1f7

#define ATA_CMD_WRITE_PIO 0x30
#define ATA_STATUS_BSY 0x80
#define ATA_STATUS_DRQ 0x08

#define ATA_DRIVE_SLAVE 0xb0
#define ATA_DRIVE_MASTER 0xf0

extern void ata_read_sector(uint32_t, uint8_t *); /* first arg is LBA Sector, second is buffer */
extern void ata_write_sector(uint32_t, uint8_t *);
