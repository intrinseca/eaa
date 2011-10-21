#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cdrom.h"
#include "stopwatch.h"

#define RANDOM_ACCESS_MAX_REPS	500
#define RANDOM_ACCESS_READ_LENGTH	1
#define SEEK_RES 3000

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

	//g_print("\treading %d sectors from offset sector %d\n", (*params).count, (*params).sector);

	cdrom_read(&cd, (*params).sector, (*params).count);
}

void spinUp()
{
	g_print("Spinning up Drive...\n");
	cdrom_set_speed(&cd, 1);
	cdrom_read(&cd, 0, 1000);
	cdrom_clear_cache(&cd);
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

void randomAccessTest(int reps, int verbose)
{
	int i;

	struct timedReadParams params;

	int start;
	double timeTaken;

	int starts[RANDOM_ACCESS_MAX_REPS];
	double times[RANDOM_ACCESS_MAX_REPS];

	spinUp();

	g_print("Beginning test (%d repetitions)\n", reps);

	for(i = 0; i < reps; i++)
	{
		start = g_random_int_range(0, CDROM_NUM_SECTORS - RANDOM_ACCESS_READ_LENGTH);

		params.sector = start;
		params.count = RANDOM_ACCESS_READ_LENGTH;

		timeTaken = stopwatch(timedRead, &params);

		if(verbose)
		{
			g_print("\ttest %d:\n", i + 1);
			g_print("\t\tstart: %d\n", start);
			g_print("\t\ttime: %f\n", timeTaken);
		}
		else
		{
			g_print(".");
		}

		starts[i] = start;
		times[i] = timeTaken;
	}

	if(!verbose)
	{
		g_print("\n");
	}

	for( i = 0; i < reps; i++)
	{
		fprintf(output, "%d,%f\n", starts[i], times[i]);
	}
}

void seekTest()
{
	int i;
	struct timedReadParams params;

	double timeTaken;

	int starts[CDROM_NUM_SECTORS / SEEK_RES];
	double times[CDROM_NUM_SECTORS / SEEK_RES];

	cdrom_set_speed(&cd, 0);
	spinUp();

	g_print("Beginning Test...\n");

	for(i = 1; i < CDROM_NUM_SECTORS / SEEK_RES; i ++)
	{
		params.sector = i * SEEK_RES;
		params.count = 10;

		cdrom_clear_cache(&cd);
		cdrom_seek(&cd, 0);

		timeTaken = stopwatch(timedRead, &params);

		g_print(".");

		starts[i] = i * SEEK_RES;
		times[i] = timeTaken;
	}

	g_print("\n");

	for( i = 0; i < CDROM_NUM_SECTORS / SEEK_RES; i++)
	{
		fprintf(output, "%d,%f\n", starts[i], times[i]);
	}
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

		if(randomReps <= RANDOM_ACCESS_MAX_REPS)
		{
			if(randomReps > 20)
			{
				randomAccessTest(randomReps, FALSE);				
			}
			else
			{
				randomAccessTest(randomReps, TRUE);						
			}
		}
		else
		{
			g_print("Too Many Reps\n");
		}
	}
	else if(strcmp(argv[1], "seek") == 0)
	{
		seekTest();
	}
	else
	{
		g_print("Usage: eaa (speed|random count)\n");
	}

	fclose(output);
	cdrom_close(&cd);

    return 0;
}