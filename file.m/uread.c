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
 * sequential read microbenchmark
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <eprintf.h>
#include <hogmem.h>
#include <mystdlib.h>
#include <puny.h>
#include <style.h>

enum {	/* Fraction of total memory to use for file size (1/n) */
	FRACTION_OF_MEMORY = 4,
	/* Fraction of file size for how much memory to leave free (1/n) */
	FRACTION_OF_FILE_SIZE = 8 };

u64 Bufsize_log2 = 12;

void usage (void)
{
	pr_usage("-f<file_name> -z<file_size> -i<num_iterations>"
		" -b<bufsize_log2> -l<loops>");
}

void fill_file (int fd, u64 size)
{
	ssize_t		written;
	size_t		toWrite;
	unsigned	i;
	u64		rest;
	u8		buf[1<<12];

	for (i = 0; i < sizeof(buf); ++i)
	{
		buf[i] = random();
	}
	for (rest = size; rest; rest -= written) {
		if (rest > sizeof(buf)) {
			toWrite = sizeof(buf);
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
	fsync(fd);
	lseek(fd, 0, 0);
}

bool myopt (int c)
{
	switch (c) {
	case 'b':
		Bufsize_log2 = strtoll(optarg, NULL, 0);
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
	ssize_t		haveRead;
	size_t		toRead;
	unsigned	i;
	unsigned	bufsize;
	unsigned	n;
	u64		size;
	u64		rest;
	u64		l;

	drop_caches();
	Option.file_size = 0;
	punyopt(argc, argv, myopt, "b:");
	n = Option.iterations;
	bufsize = 1 << Bufsize_log2;
	buf = emalloc(bufsize);
	size = Option.file_size;
	if (!size) {
		size = memtotal() / FRACTION_OF_MEMORY;
	}
	hog_leave_memory(size / FRACTION_OF_FILE_SIZE);
	fd = open(Option.file, O_RDWR | O_CREAT | O_TRUNC, 0666);
	fill_file(fd, size);
	for (l = 0; l < Option.loops; l++) {
		startTimer();
		for (i = 0; i < n; ++i) {
			for (rest = size; rest; rest -= haveRead) {
				if (rest > bufsize) {
					toRead = bufsize;
				} else {
					toRead = rest;
				}
				haveRead = read(fd, buf, toRead);
				if (haveRead != toRead) {
					if (haveRead == -1) {
						perror("read");
						exit(1);
					}
					fprintf(stderr,
						"toRead=%llu != haveRead=%lld\n",
						(u64)toRead, (s64)haveRead);
					exit(1);
				}
			}
			lseek(fd, 0, 0);
		}
		stopTimer();
		printf("size=%lld n=%d ", size, n);
		prTimer();

		printf("\t%6.4g MiB/s",
			(double)(n * size) / get_avg() / MEBI);

		printf("\n");
	}
	close(fd);
	unlink(Option.file);
	return 0;
}
