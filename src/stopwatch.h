/*
 * stopwatch.h
 *
 *  Created on: 20 Oct 2011
 *      Author: jamesbuckley
 */

#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <stdint.h>

double stopwatch(void (*ptr)(void *args), void *args);
double stopwatch_repeat(uint32_t count, double *deviation, void (*ptr)(void *args), void *args);

#endif /* STOPWATCH_H_ */
