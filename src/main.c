#include <glib.h>
#include "cdrom.h"

int main(int argc, char *argv[])
{
	cdrom_t cd;

	cdrom_init(&cd);

	cdrom_open(&cd, "/dev/sr0");

	cdrom_read(&cd, 0, 10000);

	cdrom_close(&cd);

    return 0;
}
