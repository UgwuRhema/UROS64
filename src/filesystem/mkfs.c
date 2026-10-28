#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define SECTOR_SIZE 512
#define VOLUME_START_LBA 256

/*we match the exact one used in mkfs.nothfs*/
struct nothfs_superblock {
	uint8_t signature[8];
	uint32_t version;
	uint32_t sector_size;
	uint64_t total_sectors;
	uint64_t bitmap_start;/*rel to volume start*/
	uint64_t bitmap_sectors;
	uint64_t root_start;/*rel to volume start*/
	uint64_t unique_id;
	uint8_t reserved[512 - 56];/*padding*/
} __attribute__((packed));

int main(int argc, char *argv[]) {
	char *image_name;/*general*/
	if (argc > 1)
		image_name = argv[1];
	else
		image_name = "uros.img";/*default*/
    FILE *img = fopen(image_name, "r+b"); /*we shouldn't be defaulting to just one image*/
    if(!img){
		fprintf(stderr, "Failed to find image %s\n", image_name);
		exit(1);
	}
    // Seek to Volume 1 (LBA 256)
    fseek(img, VOLUME_START_LBA * SECTOR_SIZE, SEEK_SET);
    struct nothfs_superblock sb;/*the superblock struct*/
    memset(&sb, 0, sizeof(sb));
    memcpy(sb.signature, "NOTHFS", 6);/*label signature*/
    sb.version = 1;
    sb.sector_size = SECTOR_SIZE;/*each sector size*/
    sb.total_sectors = (10 * 1024 * 1024 / 512) - VOLUME_START_LBA; 
    sb.bitmap_start = 1; // LBA 1 of the volume (which is LBA 257 of the disk)
    sb.bitmap_sectors = 5; // Enough for 20,000 sectors (1 bit per sector)
    sb.root_start = 1 + sb.bitmap_sectors; // After the bitmap
    sb.unique_id = 0x12345678;
    fwrite(&sb, sizeof(sb), 1, img);
    fclose(img);
    printf("NothFS Superblock written to LBA %d\n", VOLUME_START_LBA);
    return 0;
}
