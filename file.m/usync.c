/*
 * Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Does time test of sync for creating files, reading files
 * and deleting files.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <eprintf.h>
#include <esys.h>
#include <puny.h>
#include <style.h>
#include <timer.h>
#include <twister.h>
#include <util.h>

enum {	MAX_NAME = 12,
	DEFAULT_FILE_SIZE = 1 << 14,
	BYTES_TO_READ = 1,
	NUM_START_FILES = 1 << 9,
	NUM_FILES = 1 << 18 };

struct file {
	char name[MAX_NAME];
};

static void cleanup(void);

struct file *File;
u64 Num_files = NUM_FILES;
u64 Num_bytes_to_read = BYTES_TO_READ;
bool Prompt = FALSE;

static void prompt(const char *label)
{
	int c;

	if (Prompt) {
		printf("%s > ", label);
		for (;;) {
			c = getchar();
			switch (c) {
			case EOF:
			case 'q':
				cleanup();
				exit(0);
				break;
			case 'p':
				Prompt = FALSE;
				break;
			case '\n':
				return;
			default:
				/* ignore;  */
				break;
			}
		}
	}
}

static void fill(u8 *buf, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		buf[i] = twister_random();
	}
}

static void createfiles(int num_files)
{
	u8 *buf = emalloc(Option.file_size);
	int fd;
	int i;

	fill(buf, Option.file_size);
	for (i = 0; i < num_files; i++) {
		gen_name(File[i].name, MAX_NAME);
		fd = ecreat(File[i].name);
		ewrite(fd, buf, Option.file_size);
		eclose(fd);
	}
	free(buf);
}

static void unlinkfiles(int num_files)
{
	int i;

	for (i = 0; i < num_files; i++) {
		eunlink(File[i].name);
	}
}

static void readfiles(int num_files)
{
	u64 bytes_to_read = Num_bytes_to_read;
	u8 *buf;
	int fd;
	int i;

	if (bytes_to_read > Option.file_size)
		bytes_to_read = Option.file_size;
	buf = emalloc(bytes_to_read);
	for (i = 0; i < num_files; i++) {
		fd = eopen(File[i].name, O_RDONLY);
		eread(fd, buf, bytes_to_read);
		eclose(fd);
	}
	free(buf);
}

static void time_sync(const char *label, int n)
{
	u64 start;
	u64 finish;

	start = nsecs();
	esync();
	finish = nsecs();
	printf("%10s %8d. %10.2f ms\n",
		label, n, (double)(finish - start)/1000000.0);
	prompt("sync done");
}

void crsyncdel(int n)
{
	createfiles(n);
	prompt("create done");
	time_sync("create", n);
	readfiles(n);
	prompt("read done");
	time_sync("read", n);
	unlinkfiles(n);
	prompt("unlink done");
	time_sync("unlink", n);
}

static void cleanup(void)
{
	static bool	cleaning_up = FALSE;

	char	cmd[1024];
	int	rc;

	if (!Option.cleanup || cleaning_up) return;
	cleaning_up = TRUE;
	echdir("..");
	rc = snprintf(cmd, sizeof(cmd), "rm -fr %s", Option.dir);
	if (rc >= sizeof(cmd)) {
		eprintf("counldn't cleanup %s", cmd);
	}
	esystem(cmd);
}

static void setup(void)
{
	emkdir(Option.dir);
	echdir(Option.dir);
	File = ezalloc(sizeof(*File) * Num_files);
	set_cleanup(cleanup);
}

void usage(void)
{
	pr_usage("-p -d<directory> -n<number of files> -z<size in bytes>\n"
		"  Times sync after creating files, reading them and then\n"
		"  deleting them. Uses powers of 2 starting with %d.\n"
		"\td - directory to create for files\n"
		"\tn - maximum number files to create\n"
		"\tp - prompt after each set of operations\n"
		"\tz - size bytes of each file",
		NUM_START_FILES);
}

bool myopt(int c)
{
	switch (c) {
	case 'n':
		Num_files = strtoll(optarg, NULL, 0);
		break;
	case 'p':
		Prompt = TRUE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int main(int argc, char *argv[])
{
	int i;

	Option.file_size = DEFAULT_FILE_SIZE;

	punyopt(argc, argv, myopt, "n:p");
	setup();

	esync();
	/*
	 * Number of files grows by powers of two until the
	 * specified number of files is reached. I start
	 * at a large enough number so I see more than just
	 * noise.
	 */
	for (i = NUM_START_FILES; i <= Num_files; i <<= 1) {
		crsyncdel(i);
	}
	cleanup();
	return 0;
}
