#include <glib.h>
#include "cdrom.h"
#include "stopwatch.h"

cdrom_t cd;

void myfunc(void *ptr)
{
	cdrom_read(&cd, 0, 1000);
	cdrom_seek(&cd, CDROM_NUM_SECTORS - 1);
	cdrom_clear_cache(&cd);
}

int main(int argc, char *argv[])
{
	cdrom_init(&cd);

	cdrom_open(&cd, "/dev/sr0");

	g_print("time taken: %f\n", stopwatch(myfunc, NULL));

	cdrom_close(&cd);

    return 0;
}
