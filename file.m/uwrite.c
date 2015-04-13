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

/*
 * sequential write microbenchmark
 */

/* #define _GNU_SOURCE */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <eprintf.h>
#include <hogmem.h>
#include <mystdlib.h>
#include <puny.h>
#include <style.h>

enum {	/* Fraction of total memory to use for file size (1/n) */
	FRACTION_OF_MEMORY = 4,
	/* Fraction of file size for how much memory to leave free (1/n) */
	FRACTION_OF_FILE_SIZE = 2 };

u64 Bufsize_log2 = 12;
bool Hog_memory = TRUE;
bool Direct = FALSE;

void usage (void)
{
	pr_usage("-m -f<file_name> -z<file_size> -i<num_iterations>"
		" -b<bufsize_log2> -l<loops>\n"
		"  -m - turns off memory hog\n"
		"  -d - use O_DIRECT access (disables memory Hog)\n"
		"  -f - path name of file\n"
		"  -z - file size in bytes\n"
		"  -b - buffer size - base 2 - 12->4096\n"
		"  -i - number of inner iterations for one test\n"
		"  -l - number of times to rerun test - results are averaged");
}

bool myopt (int c)
{
	switch (c) {
	case 'b':
		Bufsize_log2 = strtoll(optarg, NULL, 0);
		break;
	case 'd':
		Direct = TRUE;
		Hog_memory = FALSE;
		break;
	case 'm':
		Hog_memory = FALSE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int main (int argc, char *argv[])
{
	u8		*buf;
	int		fd;
	ssize_t		written;
	size_t		toWrite;
	unsigned	i;
	unsigned	bufsize;
	unsigned	n;
	u64		size;
	u64		rest;
	u64		l;

	drop_caches();
	Option.file_size = 0;
	Option.iterations = 1;
	Option.loops = 1;
	punyopt(argc, argv, myopt, "mdb:");

	n = Option.iterations;
	bufsize = 1 << Bufsize_log2;
	buf = emalloc(bufsize);
	for (i = 0; i < bufsize; ++i) {
		buf[i] = random();
	}
	size = Option.file_size;
	if (!size) {
		size = memtotal() / FRACTION_OF_MEMORY;
	}
	if (Hog_memory) {
		hog_leave_memory(size / FRACTION_OF_FILE_SIZE);
	}
	for (l = 0; l < Option.loops; l++) {
		startTimer();
		for (i = 0; i < n; i++) {
			/*
			 * Because eCryptfs has to decrypt a page before
			 * overwriting it, recreating the file on each
			 * iteration gives a more realistic value.
			 */
			int flags = O_RDWR | O_CREAT | O_TRUNC;
			if (Direct)
				flags |= O_DIRECT;
			fd = open(Option.file,
				  flags, 0666);
			if (fd == -1) fatal("open %s:", Option.file);
			for (rest = size; rest; rest -= written) {
				if (rest > bufsize) {
					toWrite = bufsize;
				} else {
					toWrite = rest;
				}
				written = write(fd, buf, toWrite);
				if (written != toWrite) {
					if (written == -1) {
						perror("write");
						exit(1);
					}
					fprintf(stderr,
						"toWrite=%lu != written=%ld\n",
						(unint)toWrite, (snint)written);
					exit(1);
				}
				/* Make next buffer unique */
				buf[urand(bufsize)] = random();
			}
			/*
			 * Catch-22 - have to do fsync but fsync triggers
			 * I/O-less throttling which slows I/Os down
			 */
			fsync(fd);
			close(fd);
		}
		stopTimer();
		printf("size=%lld n=%d ", size, n);
		prTimer();

		printf("\t%6.4g MiB/s\n",
			(double)(n * size) / get_avg() / MEBI);
	}
	unlink(Option.file);
	return 0;
}
