#include <stdio.h>
#include <stdlib.h>
#include "../src/filesystem/nothfs.h"
#include <string.h>

#define SECTOR_SIZE 512
#define SPECIFIED_LBA 263

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
	new_file->size = 12;
	new_file->datetime = 17112026;
	fseek(fd, SECTOR_SIZE * SPECIFIED_LBA, SEEK_SET);/*go to ba 267 on the disk*/
	fwrite(new_file, sizeof(struct nothfs_dirent_t), 1, fd);
	printf("New Nothfs_dirent written to LBA %d of name %s\n", SPECIFIED_LBA, new_file->name);
	free(new_file);
	fclose(fd);
	return 0;
}
