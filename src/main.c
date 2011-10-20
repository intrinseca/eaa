#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cdrom.h"
#include "stopwatch.h"

#define RANDOM_ACCESS_MAX_REPS	500
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

void randomAccessTest(int reps)
{
	int i;

	struct timedReadParams params;

	int start;
	double timeTaken;
	double speed;

	int starts[RANDOM_ACCESS_MAX_REPS];
	double times[RANDOM_ACCESS_MAX_REPS];
	double mean;

	g_print("Spinning up Drive...\n");
	cdrom_set_speed(&cd, 52);
	cdrom_read(&cd, 0, 1000);
	cdrom_clear_cache(&cd);

	g_print("Beginning test (%d repetitions)\n", reps);

	cdrom_seek(&cd, 0);

	for(i = 0; i < reps; i++)
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


	for( i = 0; i < reps; i++)
	{
		fprintf(output, "%d,%f\n", starts[i], times[i]);
		mean += starts[i] / times[i];
	}

	mean /= reps;
	g_print("mean speed: %f\n", mean);
}

int main(int argc, char *argv[])
{
	int randomReps = RANDOM_ACCESS_MAX_REPS;

	srand(time(NULL));

	output = fopen("results.csv", "w");
	cdrom_init(&cd);

	cdrom_open(&cd, "/dev/sr0");

	if(strcmp(argv[1], "speed") == 0)
	{
		speedTest(NULL);
	}
	else if(strcmp(argv[1], "random") == 0)
	{
		randomReps = atoi(argv[2]);

		if(randomReps < RANDOM_ACCESS_MAX_REPS)
		{
			randomAccessTest(randomReps);
		}
		else
		{
			g_print("Too Many Reps\n");
		}
	}
	else
	{
		g_print("Usage: eaa (speed|random count)\n");
	}

	fclose(output);
	cdrom_close(&cd);

    return 0;
}