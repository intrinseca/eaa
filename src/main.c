#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cdrom.h"
#include "stopwatch.h"

#define RANDOM_ACCESS_REPS	200
#define RANDOM_ACCESS_READ_LENGTH	1

cdrom_t cd;
FILE *output;

void randomAccessRead(void *ptr)
{
	int *start = ptr;

	g_print("\treading %d sectors from offset %d\n", RANDOM_ACCESS_READ_LENGTH, *start);
	cdrom_read(&cd, *start, RANDOM_ACCESS_READ_LENGTH);
	cdrom_clear_cache(&cd);
}

void speedTest()
{
	int start = 0;
	int i;

	for(i = 1; i < 52; i++)
	{
		g_print("Spinning up Drive to %dx...\n", i);
		cdrom_set_speed(&cd, i);
		cdrom_read(&cd, 0, 1000);
		cdrom_clear_cache(&cd);

		g_print("\t time: %f\n", stopwatch(randomAccessRead, &start));
	}
}

void randomAccessTest(void *ptr)
{
	int i;
	float nextRand;

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
		//nextRand = (float)rand() / RAND_MAX;
		//start = nextRand * (CDROM_NUM_SECTORS - RANDOM_ACCESS_READ_LENGTH);
		start = g_random_int_range(0, CDROM_NUM_SECTORS - RANDOM_ACCESS_READ_LENGTH);

		starts[i] = start;

		timeTaken = stopwatch(randomAccessRead, &start);
		speed = start / timeTaken;

		g_print("\ttest %d:\n", i + 1);
		g_print("\t\tstart: %d\n", start);
		g_print("\t\ttime: %f\n", timeTaken);
		g_print("\t\tspeed: %f\n", speed);

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

	g_print("total time taken: %fs\n", stopwatch(speedTest, NULL));

	fclose(output);
	cdrom_close(&cd);

    return 0;
}