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
 * Random read microbenchmark. Creates a large file and randomly reads
 * blocks form the file. The file should be large enough that most
 * of it does not fit in cache so there is a high probablity of a
 * buffer cache miss.
 * -z <file_size> should be much larger than buffer cache
 * -b <bufsize_log2> block size that will be used to read file
 *	typically 12 which is 4096
 */

#define _XOPEN_SOURCE 600

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
	pr_usage("-f<file_name> -z<file_size> -i<num_iterations>"
		" -b<bufsize_log2> -l<loops>\n"
		"  -f - path name of file\n"
		"  -d - use O_DIRECT access (disables memory Hog)\n"
		"  -z - file size in bytes\n"
		"  -b - buffer size - base 2 - 12->4096\n"
		"  -i - number of inner iterations for one test\n"
		"  -l - number of times to rerun test - results are averaged");
}

void fill_file (int fd, u64 size)
{
	ssize_t		written;
	size_t		toWrite;
	unsigned	i;
	u64		rest;
	u8		*buf;

	/* align the buffer in case we are using O_DIRECT */
	buf = aligned_alloc(4096, 4096);
	if (!buf) {
		fprintf(stderr, "unable to allocate aligned memory\n");
		exit(1);
	}
	for (i = 0; i < 4096; ++i)
	{
		buf[i] = random();
	}
	for (rest = size; rest; rest -= written) {
		if (rest > 4096) {
			toWrite = 4096;
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
				"toWrite=%llu != written=%lld\n",
				(u64)toWrite, (s64)written);
			exit(1);
		}
	}
	free(buf);
	fsync(fd);
	lseek(fd, 0, 0);
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
	default:
		return FALSE;
	}
	return TRUE;
}

int main (int argc, char *argv[])
{
	u8		*buf;
	int		fd;
	int             flags;
	unsigned	i;
	unsigned	bufsize;
	unsigned	n;
	u64		size;
	u64		numbufs;
	u64		offset;
	u64		l;
	ssize_t		rc;

	Option.file_size = memtotal() / FRACTION_OF_MEMORY;
	Option.iterations = 10000;
	Option.loops = 2;
	punyopt(argc, argv, myopt, "db:");
	n = Option.iterations;
	size = Option.file_size;
	if (Hog_memory) {
		hog_leave_memory(size / FRACTION_OF_FILE_SIZE);
	}
	bufsize = 1 << Bufsize_log2;
	/* align to bufsize in case of O_DIRECT */
	buf = aligned_alloc(bufsize, bufsize);
	if (!buf) {
		fprintf(stderr, "unable to allocate aligned memory\n");
		exit(1);
	}

	numbufs = size / bufsize;

	flags = O_RDWR | O_CREAT | O_TRUNC;
	if (Direct) {
		flags |= O_DIRECT;
	}
	fd = open(Option.file, flags, 0666);
	fill_file(fd, size);
	for (l = 0; l < Option.loops; l++) {
		startTimer();
		for (i = 0; i < n; ++i) {
			offset = urand(numbufs) * bufsize;
			rc = pread(fd, buf, bufsize, offset);
			if (rc != bufsize) {
				if (rc == -1) fatal("pread:");
				fatal("pread rc=%d offset=%lld", rc, offset);
			}
		}
		stopTimer();
		printf("size=%lld n=%d ", size, n);
		prTimer();

		printf("\t%6.4g MiB/s",
			(double)(n * bufsize) / get_avg() / MEBI);

		printf("\t%6.4g IOPs/sec",
			(double)(n) / get_avg());

		printf("\n");
	}
	free(buf);
	close(fd);
	unlink(Option.file);
	return 0;
}
