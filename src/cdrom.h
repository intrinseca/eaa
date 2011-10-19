/*
 * cdrom.h
 *
 *  Created on: 19 Oct 2011
 *      Author: jamesbuckley
 */

#ifndef CDROM_H_
#define CDROM_H_

#include <stdint.h>

#define CDROM_SECTOR_SIZE	2048
#define CDROM_NUM_SECTORS	333000

typedef struct {
	int fd;
	uint32_t curr_sector;
	uint32_t *scratch_buf;
} cdrom_t;

int cdrom_init(cdrom_t *cd);
int cdrom_open(cdrom_t *cd, char *path);
int cdrom_read(cdrom_t *cd, uint32_t sector, uint32_t count);

#endif /* CDROM_H_ */
