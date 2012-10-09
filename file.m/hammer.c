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

enum { FILE_SIZE_MEG = (1<<12) };

u8	Buf[1<<12];
unint	Bufs_written;
bool	Done = FALSE;
bool	Keep = FALSE;

void pr_human_bytes(u64 x)
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
	printf("%6.1f %s", (double)x / y, suffix);
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

void *timer(void *arg)
{
	struct timespec sleep = { Option.sleep_secs, 0 * ONE_MILLION };
	unint old_bufs_written;
	u64 delta;
	u64 i;

	for (i = 0; !Done; i++) {
		old_bufs_written = Bufs_written;
		nanosleep(&sleep, NULL);
		delta = Bufs_written - old_bufs_written;
		printf("%4llu. ", i);
		pr_human_bytes((delta * sizeof(Buf) + Option.sleep_secs / 2) /
				Option.sleep_secs);
		printf("/sec\n");
	}
	return NULL;
}

void usage(void)
{
	pr_usage("-k -f<file> -s<secs to sleep>\n"
		"hammer writes continuously to a large file\n"
		"It does an fsync when it reaches the specified\n"
		"size then starts over. It displays the write rate\n"
		"in seconds\n"
		"\tk - keep file used to hammer the disk\n"
		"\tf - file path to hammer the disk\n"
		"\ts - seconds to sleep between reports\n"
		"\tz - size in Mibs of file to hammer system");
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
