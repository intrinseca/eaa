/*
 * cdrom.c
 *
 *  Created on: 19 Oct 2011
 *      Author: jamesbuckley
 */
#include "cdrom.h"

#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>

int cdrom_init(cdrom_t *cd)
{
	cd->curr_sector = 0;
	cd->fd = -1;

	posix_memalign(&(cd->scratch_buf), CDROM_SECTOR_SIZE, CDROM_SECTOR_SIZE);

	return 0;
}

int cdrom_open(cdrom_t *cd, char *path)
{
	cd->fd = open(path, O_RDONLY | O_DIRECT);

	if(cd->fd == -1) {
		g_printerr("failed to open %s\n", path);
		return -1;
	}

	g_print("opened %s\n", path);
	return 0;
}

int cdrom_close(cdrom_t *cd, char *path)
{
	close(cd->fd);

	return 0;
}

int cdrom_read(cdrom_t *cd, uint32_t sector, uint32_t count)
{
	int ret;
	uint32_t byte_offset;
	uint32_t sectors_remaining;

	byte_offset = sector * CDROM_SECTOR_SIZE;

	g_print("reading %d sectors from offet %d\n", count, sector);

	lseek(cd->fd, byte_offset, SEEK_SET);

	while(count > 0) {
		ret = read(cd->fd, cd->scratch_buf, CDROM_SECTOR_SIZE);

		if(ret < 0) {
			/* an error occured */
				g_print("error: ");
				perror("");
		}
		count--;
	}
}
