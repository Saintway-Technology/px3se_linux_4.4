/****************************************************************************
 |  (C) Copyright 2008 Novell, Inc. All Rights Reserved.
 |
 |  GPLv2: This program is free software; you can redistribute it
 |  and/or modify it under the terms of version 2 of the GNU General
 |  Public License as published by the Free Software Foundation.
 |
 |  This program is distributed in the hope that it will be useful,
 |  but WITHOUT ANY WARRANTY; without even the implied warranty of
 |  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 |  GNU General Public License for more details.
 +-------------------------------------------------------------------------*/

#include <sys/time.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <puny.h>

enum { NUM_TIMERS = 2048 };

typedef struct Timer_s
{
	void	*key;
	struct timeval	start;
	struct timeval	delta;
} Timer_s;

Timer_s		Timers[NUM_TIMERS];
Timer_s		*NextTimer = Timers;
unsigned	TimerNotFound;

void startTimer (void *key)
{
	Timer_s	*timer;

	timer = NextTimer++;
	if (NextTimer == &Timers[NUM_TIMERS])
	{
		NextTimer = Timers;
	}
	timer->key = key;
	gettimeofday( &timer->start, NULL);
}

void endTimer (void *key)
{
	struct timeval	end;
	Timer_s			*timer;

	gettimeofday( &end, NULL);
	for (timer = NextTimer - 1; timer >= Timers; --timer)
	{
		if (timer->key == key)
		{
			goto found;
		}
	}
	for (timer = &Timers[NUM_TIMERS - 1]; timer != NextTimer; --timer)
	{
		if (timer->key == key)
		{
			goto found;
		}
	}
	printf("Timer not found for key = %p\n", key);
	++TimerNotFound;
	return;

found:
	if (end.tv_usec < timer->start.tv_usec)
	{
			--end.tv_sec;
			end.tv_usec += 1000000;
	}
	timer->delta.tv_sec  = end.tv_sec - timer->start.tv_sec;
	timer->delta.tv_usec = end.tv_usec - timer->start.tv_usec;
}

static void printTimer (Timer_s *timer)
{
	if (timer->key == 0) return;
	/* We cannot use 'long' as some 32bit systems define time_t as
	 * as 64bit value.  Others define it as a 32bit value.  So we
	 * have to explicitly cast it ourselves to get a stable printf
	 * format string.
	 */
	printf("%8p: start=%"PRIu64".%.6"PRIu64" delta=%"PRIu64".%.6"PRIu64"\n",
				timer->key,
				(uint64_t)timer->start.tv_sec,
				(uint64_t)timer->start.tv_usec,
				(uint64_t)timer->delta.tv_sec,
				(uint64_t)timer->delta.tv_usec);
}

void dumpTimersReverse (void)
{
	Timer_s			*timer;

	for (timer = NextTimer - 1; timer >= Timers; --timer)
	{
		printTimer(timer);
	}
	for (timer = &Timers[NUM_TIMERS - 1]; timer != NextTimer; --timer)
	{
		printTimer(timer);
	}
}

void dumpTimers (void)
{
	Timer_s			*timer;

	for (timer = NextTimer; timer < &Timers[NUM_TIMERS]; ++timer)
	{
		printTimer(timer);
	}
	for (timer = Timers; timer != NextTimer; ++timer)
	{
		printTimer(timer);
	}
}

unsigned long urand (unsigned long upper)
{
	return upper ? (random() % upper) : 0;
}

int main (int argc, char *argv[])
{
	void	*key;
	int	i;

	punyopt(argc, argv, NULL, NULL);
	for (i = 0; i < 10; ++i)
	{
		key = (void *)urand(200000);

		startTimer(key);
		usleep((long)key);
		endTimer(key);
	}
	dumpTimers();
	return 0;
}
