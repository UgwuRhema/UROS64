#include <stdio.h>
#include <stdlib.h>
#include "../src/filesystem/nothfs.h"
#include <string.h>

#define SECTOR_SIZE 512
#define SPECIFIED_LBA 263
#define DEF_VOL 256

int
main(int argc, char *argv[])
{
	/*we edit a new nothfs_dirent into a file*/
	char *default_image_filename = NULL;
	if (argc > 1)
		default_image_filename = argv[1];
	else
		default_image_filename = "../uros.img";
	FILE *fd = fopen(default_image_filename, "r+b");
	/*let's create the new nothfs_dirent*/
	struct nothfs_dirent_t *new_file = (struct nothfs_dirent_t *)malloc(sizeof(struct nothfs_dirent_t));
	memset(new_file, 0, sizeof(struct nothfs_dirent_t));/*clean memory*/
	new_file->attr = NOTHFS_ATTR_CONTIG;/*file attribute*/
	memcpy(new_file->name, "payload.txt\0", 12);
	new_file->clus = 31;
	new_file->size = 512;/*no data yet*/
	new_file->datetime = 17112026;
	printf("New Nothfs_dirent written to LBA %d of name %s\n", SPECIFIED_LBA, new_file->name);
	fseek(fd, 0, SEEK_SET);
	fseek(fd, SECTOR_SIZE * (DEF_VOL + new_file->clus), SEEK_SET);/*at the file's cluster sector so we
	can write data */
	char big_text[512];/*fahh, it had a const qualifier*/
	for (int i = 0; i < 512; ++i)
		big_text[i] = 'A';/*what?!*/
	fwrite(big_text, 512, 1, fd);
	fseek(fd, SECTOR_SIZE * SPECIFIED_LBA, SEEK_SET);/*go to ba 267 on the disk*/
	fwrite(new_file, sizeof(struct nothfs_dirent_t), 1, fd);
	free(new_file);
	fclose(fd);
	return 0;
}
