#include "../func/func.h"



extern void ata_read_sector(uint64_t, uint8_t *); /* first arg is LBA Sector, second is buffer */
extern void ata_write_sector(uint64_t, uint8_t *);
