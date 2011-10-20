#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cdrom.h"
#include "stopwatch.h"

#define RANDOM_ACCESS_REPS	10
#define RANDOM_ACCESS_READ_LENGTH	1

cdrom_t cd;
FILE *output;

struct timedReadParams
{
	int sector;
	int count;
};

void timedRead(void *ptr)
{
	struct timedReadParams *params = ptr;

	g_print("\treading %d sectors from offset sector %d\n", (*params).count, (*params).sector);
	
	cdrom_read(&cd, (*params).sector, (*params).count);
}

void speedTest()
{
	int i;

	struct timedReadParams params;

	for(i = 1; i <= 52; i *= 2)
	{
		g_print("Spinning up Drive to %dx...\n", i);
		cdrom_set_speed(&cd, i);
		cdrom_read(&cd, 0, 1000);
		cdrom_clear_cache(&cd);

		params.sector = 0;
		params.count = 1000;

		g_print("\ttime: %f\n", stopwatch(timedRead, &params));
	}
}

void randomAccessTest(void *ptr)
{
	int i;

	struct timedReadParams params;

	int start;
	double timeTaken;
	double speed;

	int starts[RANDOM_ACCESS_REPS];
	double times[RANDOM_ACCESS_REPS];
	double mean;

	g_print("Spinning up Drive...\n");
	cdrom_set_speed(&cd, 52);
	cdrom_read(&cd, 0, 1000);
	cdrom_clear_cache(&cd);

	g_print("Beginning test...\n");

	cdrom_seek(&cd, 0);

	for( i = 0; i < RANDOM_ACCESS_REPS; i++)
	{
		start = g_random_int_range(0, CDROM_NUM_SECTORS - RANDOM_ACCESS_READ_LENGTH);

		params.sector = start;
		params.count = RANDOM_ACCESS_READ_LENGTH;

		timeTaken = stopwatch(timedRead, &params);
		speed = start / timeTaken;

		g_print("\ttest %d:\n", i + 1);
		g_print("\t\tstart: %d\n", start);
		g_print("\t\ttime: %f\n", timeTaken);
		g_print("\t\tspeed: %f\n", speed);

		starts[i] = start;
		times[i] = timeTaken;
	}


	for( i = 0; i < RANDOM_ACCESS_REPS; i++)
	{
		fprintf(output, "%d,%f\n", starts[i], times[i]);
		mean += starts[i] / times[i];
	}

	mean /= RANDOM_ACCESS_REPS;
	g_print("mean speed: %f\n", mean);
}

int main(int argc, char *argv[])
{
	srand(time(NULL));

	output = fopen("results.csv", "w");
	cdrom_init(&cd);

	cdrom_open(&cd, "/dev/sr0");

	speedTest(NULL);
	randomAccessTest(NULL);

	fclose(output);
	cdrom_close(&cd);

    return 0;
}