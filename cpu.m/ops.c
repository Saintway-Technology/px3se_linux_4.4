/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Measure time to perform operations like divide from C on one cpu.
 *
 * Because the test measures times for short operations like divide,
 * the timing must account for the overhead for the loop and extra
 * code from preventing the optimizer from removing the code that
 * is being measured.
 *
 * time overhead
 * time op
 * result = op - overhead
 */

#include <sys/resource.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <debug.h>
#include <eprintf.h>
#include <puny.h>
#include <style.h>
#include <timer.h>
#include <twister.h>

typedef u64 (*op_f)(u64 x0, u64 y0, u64 incx, u64 incy, size_t n);

u64 Overhead;

/*
 * overhead is used to time the loop and code to avoid
 * optimization.
 */
u64 overhead (u64 x0, u64 y0, u64 incx, u64 incy, size_t n)
{
	u64	sum = 0;
	u64	q = 0;
	u64	x = x0;
	u64	y = y0;

	while (n-- != 0) {
		if (!y) y = 1;		/* to avoid divide by 0 */
		q = x + y;		/* op being timed */
		x += incx;		/* change x and y */
		y += incy;
		sum += q + incx + incy;	/* sum so we have to return a result
					 * that can only be computed by doing
					 * everything in the loop.
					 */
	}
	return sum;
}

/*
 * 64 bit unsigned divide
 */
u64 divide64 (u64 x0, u64 y0, u64 incx, u64 incy, size_t n)
{
	u64	sum = 0;
	u64	q;
	u64	x = x0;
	u64	y = y0;

	while (n-- != 0) {
		if (!y) y = 1;
		q = x / y;
		x += incx;
		y += incy;
		sum += q + incx + incy;
	}
	return sum;
}

/*
 * 32 bit unsigned divide
 */
u64 divide32 (u64 x0, u64 y0, u64 incx, u64 incy, size_t n)
{
	u64	sum = 0;
	u32	q;
	u32	x = x0;
	u32	y = y0;

	while (n-- != 0) {
		if (!y) y = 1;
		q = x / y;
		x += incx;
		y += incy;
		sum += q + incx + incy;
	}
	return sum;
}

u64 non_zero_rand (void)
{
	u64	x;

	do {
		x = twister_random();
	} while (!x);
	return x;
}

void time_overhead (op_f f)
{
	u64	start;
	u64	finish;
	u64	x = non_zero_rand();
	u64	y = non_zero_rand();
	u64	incx = non_zero_rand();
	u64	incy = non_zero_rand();

	start = nsecs();
	f(x, y, incx, incy, Option.iterations);
	finish = nsecs();
	Overhead = finish - start;
	printf("Overhead = %llu\n", Overhead);
}

void time_loop (int j, op_f f)
{
	u64	start;
	u64	finish;
	u64	x = non_zero_rand();
	u64	y = non_zero_rand();
	u64	incx = non_zero_rand();
	u64	incy = non_zero_rand();
	u64	t;

	start = nsecs();
	f(x, y, incx, incy, Option.iterations);
	finish = nsecs();
	t = (finish - start);
	if (t <= Overhead) {
		printf("Overhead(%llu) was greater than test(%llu)\n",
			Overhead, t);
	} else {
		t -= Overhead;
	}
	printf("%d. %g nsecs/op\n", j,
		(double)(t) / (double)Option.iterations);
}

void test (char *test_name, op_f f)
{
	int		j;

	printf("%s\n", test_name);
	for (j = 0; j < Option.loops; j++) {
		time_loop(j, f);
	}
}

void RunTests (void)
{
	test("overhead", overhead);
	time_overhead(divide64);	/* This call needed to fool optimizer */
	time_overhead(overhead);
	test("divide64", divide64);
	test("divide32", divide32);
}

void usage (void)
{
	pr_usage("-h -i<iterations> -l<loops>\n"
		"\th - help\n"
		"\ti - copy buffer i times [%lld]\n"
		"\tl - number of trials to run [%lld]\n",
		Option.iterations, Option.loops);
}

int main (int argc, char *argv[])
{
	Option.iterations = 100000000;
	Option.loops = 4;
	punyopt(argc, argv, NULL, NULL);
	RunTests();
	return 0;
}
