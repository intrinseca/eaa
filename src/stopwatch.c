/*
 * stopwatch.c
 *
 *  Created on: 20 Oct 2011
 *      Author: jamesbuckley
 */

#include "stopwatch.h"

#include <math.h>
#include <glib.h>

double stopwatch(void (*ptr)(void *args), void *args)
{
	double elapsed;

	GTimer *tmr = g_timer_new();

	g_timer_start(tmr);

		ptr(args);

	g_timer_stop(tmr);

	elapsed = g_timer_elapsed(tmr, NULL);

	g_timer_destroy(tmr);

	return elapsed;
}

double stopwatch_repeat(uint32_t count, double *deviation, void (*ptr)(void *args), void *args)
{
	uint32_t i;
	double time;
	double sum;
	double mean;
	GArray *times = g_array_sized_new(FALSE, FALSE, sizeof(double), count);

	for(i = 0; i < count; i++) {
		time = stopwatch(ptr, args);

		g_array_append_val(times, time);
	}

	sum = 0;
	for(i = 0; i < count; i++) {
		sum += g_array_index(times, double, i);
	}

	mean = sum / count;

	if (deviation != NULL) {
		sum = 0;
		for(i = 0; i < count; i++) {
			sum += pow(g_array_index(times, double, i), 2);
		}

		sum /= (count - 1);

		*deviation = sqrt(sum);
	}

	g_array_free(times, TRUE);
}
