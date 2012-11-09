/*
 * Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Continually writes a large file until user breaks out of test.
 * This is to "hammer" the storage device.
 * Prints rate of writes.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <style.h>
#include <eprintf.h>
#include <puny.h>

enum {	FILE_SIZE_MEG = 1 << 12,
	BUF_SIZE = 1 << 12,
	WINDOW_SIZE = 20 };

u8	Buf[BUF_SIZE];
unint	Bufs_written;
bool	Done = FALSE;
bool	Keep = FALSE;

void pr_human(double x)
{
	char *suffix;
	u64 y;

	if (x >= EXBI) {
		suffix = "EiB";
		y = EXBI;
	} else if (x >= PEBI) {
		suffix = "PiB";
		y = PEBI;
	} else if (x >= TEBI) {
		suffix = "TiB";
		y = TEBI;
	} else if (x >= GIBI) {
		suffix = "GiB";
		y = GIBI;
	} else if (x >= MEBI) {
		suffix = "MiB";
		y = MEBI;
	} else if (x >= KIBI) {
		suffix = "KiB";
		y = KIBI;
	} else {
		suffix = "B";
		y = 1;
	}
	printf("%7.1f %s/sec", x / y, suffix);
}

void *writer(void *arg)
{
	int fd;
	int rc;
	u64 size = 0;
	u64 max_size = Option.file_size * MEBI;

	fd = open(Option.file, O_WRONLY | O_CREAT | O_TRUNC, 0700);
	if (fd == -1) fatal("open %s:", Option.file);
	if (!Keep) unlink(Option.file);
	for (;;) {
		rc = write(fd, Buf, sizeof(Buf));
		if (rc == -1) {
			fatal("unexpected write error %s:", Option.file);
		}
		if (rc != sizeof(Buf)) {
			fatal("didn't write all the data %d:", rc);
		}
		++Bufs_written;
		size += rc;
		if (size >= max_size) {
			fsync(fd);
			rc = lseek(fd, 0, 0);
			if (rc == -1) {
				fatal("lseek:");
			}
			size = 0;
		}
	}
	Done = TRUE;
	return NULL;
}

void pr_bytes_per_sec (double bytes_per_sec)
{
	static double window[WINDOW_SIZE] = { 0 };
	static double *next = window;
	static double num_windows = 0;
	static double window_total = 0;
	static double num = 0;
	static double total = 0;
	double window_avg;
	double avg;

	++num;
	total += bytes_per_sec;
	avg = total / num;
	if (next == &window[WINDOW_SIZE]) {
		next = window;
	}
	if (num_windows != WINDOW_SIZE) {
		++num_windows;
	}
	/*
	 * Subtract the oldest window entry from the total
	 * before overwriting it with the new bytes_per_sec.
	 */
	window_total -= *next;
	window_total += bytes_per_sec;
	*next++ = bytes_per_sec;

	window_avg = window_total / num_windows;

	pr_human(bytes_per_sec);
	pr_human(window_avg);
	pr_human(avg);
}

void *timer(void *arg)
{
	struct timespec sleep = { Option.sleep_secs, 0 * ONE_MILLION };
	unint old_bufs_written;
	u64 delta;
	u64 i;

	printf("secs      bytes/sec    window avg(%2d)      avg\n", WINDOW_SIZE);
	for (i = 0; !Done; i++) {
		old_bufs_written = Bufs_written;
		nanosleep(&sleep, NULL);
		delta = Bufs_written - old_bufs_written;
		printf("%4llu. ", i);
		pr_bytes_per_sec((double)(delta * sizeof(Buf)) /
		                 Option.sleep_secs);
		printf("\n");
	}
	return NULL;
}

void usage(void)
{
	pr_usage("-k -f<file> -s<secs to sleep> -z<size in MiBs>\n"
		"  hammer writes continuously to a large file\n"
		"  It does an fsync when it reaches the specified\n"
		"  size then starts over. It displays the write rate\n"
		"  in seconds\n"
		"\tk - keep file used to hammer the disk\n"
		"\tf - file path to hammer the disk\n"
		"\ts - seconds to sleep between reports\n"
		"\tz - size in MiBs of file to hammer system");
}

bool myopt(int c)
{
	switch (c) {
	case 'k':
		Keep = TRUE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int main(int argc, char *argv[])
{
	pthread_t timer_thread;
	pthread_t writer_thread;
	int rc;

	Option.file_size = FILE_SIZE_MEG;
	Option.sleep_secs = 1;

	punyopt(argc, argv, myopt, "k");
	rc = pthread_create(&timer_thread, NULL, timer, NULL);
	if (rc) fatal("timer thread:");
	rc = pthread_create(&writer_thread, NULL, writer, NULL);
	if (rc) fatal("writer thread:");

	pthread_join(writer_thread, NULL);
	pthread_join(timer_thread, NULL);

	return 0;
}
