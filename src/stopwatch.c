/*
 * stopwatch.c
 *
 *  Created on: 20 Oct 2011
 *      Author: jamesbuckley
 */

#include "stopwatch.h"

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
