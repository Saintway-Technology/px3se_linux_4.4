/*
 * Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Like hammer, this test stresses the file system but instead of
 * using writes to one big file, crdel uses lots of little files.
 * Uses a circular buffer of files that are created, written and
 * then deleted. The creation and deletion run synchrounous but
 * keep the circular buffer at least 90% full.
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
	MAX_FILE_SIZE = 1 << 14,
	NUM_FILES = 1 << 16 };

struct file {
	char name[MAX_NAME];
};

bool Done = FALSE;
struct file *File;
u64 Num_files = NUM_FILES;
int Next;
int Last;
unint Commited;
unint Deleted;

static bool is_empty(void)
{
	return Next == Last;
}

static bool is_full(void)
{
	int next = Next + 1;
	if (next == Num_files) {
		return Last == 0;
	} else {
		return next == Last;
	}
}

static void commit(void)
{
	if (++Next == Num_files)
		Next = 0;
	++Commited;
}

static void delete(void)
{
	if (++Last == Num_files)
		Last = 0;
	++Deleted;
}

static void doze(void)
{
	struct timespec sleep = { 0, ONE_THOUSAND /* ns */};

	nanosleep(&sleep, 0);
}

static void fill(u8 *buf, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		buf[i] = twister_random();
	}
}

void *creator(void *unused)
{
	u8 *buf = emalloc(Option.file_size);
	int fd;

	fill(buf, Option.file_size);
	while (!Done) {
		while (is_full())
			doze();
		gen_name(File[Next].name, MAX_NAME);
		fd = ecreat(File[Next].name);
		/* Can't advance Next until after file is created */
		commit();
		ewrite(fd, buf, Option.file_size);
		eclose(fd);
	}
	return NULL;
}

void *unlinker(void *unused)
{
	while (!Done) {
		while (is_empty())
			doze();
		if (((Commited - Deleted) * 100 / Num_files) > 90) {
			eunlink(File[Last].name);
			delete();
		} else {
			doze();
		}
	}
	while (!is_empty()) {
		eunlink(File[Last].name);
		delete();
	}
	return NULL;
}

void *syncer(void *unused)
{
	struct timespec sleep = { 20 /* s */, 0 /* ns */};
	u64 start;
	u64 finish;
	u64 i;

	for (i = 0; !Done; i++) {
		start = nsecs();
		esync();
		finish = nsecs();
		printf("sync: %4llu. %3.2g\n", i,
			(double)(finish - start)/ONE_BILLION);
		nanosleep(&sleep, NULL);
	}
	return NULL;
}

void *timer(void *arg)
{
	struct timespec sleep = { Option.sleep_secs, 0 /* ns */ };
	unint old_num_created;
	unint old_num_unlinked;
	unint delta_created;
	unint delta_unlinked;
	u64 i;

	printf("secs created/sec deleted/sec\n");
	for (i = 0; !Done; i++) {
		old_num_unlinked = Deleted;
		old_num_created = Commited;
		nanosleep(&sleep, NULL);
		delta_unlinked = Deleted - old_num_unlinked;
		delta_created = Commited - old_num_created;
		printf("%4llu. %10ld %10ld\n", i, delta_created, delta_unlinked);
	}
	return NULL;
}

void cleanup(void)
{
	static bool cleaning_up = FALSE;
	char cmd[1024];
	int rc;

	if (!Option.cleanup || cleaning_up)
		return;
	cleaning_up = TRUE;
	echdir("..");
	rc = snprintf(cmd, sizeof(cmd), "rm -fr %s", Option.dir);
	if (rc > sizeof(cmd) - 2) {	/* shouldn't be that big */
		eprintf("counldn't cleanup %s", cmd);
	}
	esystem(cmd);
}

void setup(void)
{
	emkdir(Option.dir);
	echdir(Option.dir);
	File = ezalloc(sizeof(*File) * Num_files);
	set_cleanup(cleanup);
}

void usage(void)
{
	pr_usage("-d<directory> -n<number of files> -z<size in bytes>\n"
		"  busy continuously creates and deletes files\n"
		"  It displays the create/delete in seconds\n"
		"\td - directory to create paths\n"
		"\tn - max files to create\n"
		"\ts - seconds to sleep between reports\n"
		"\tz - size in MiBs of file to hammer system");
}

bool myopt(int c)
{
	switch (c) {
	case 'n':
		Num_files = strtoll(optarg, NULL, 0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void epthread_create(const char *name, pthread_t *thread,
	const pthread_attr_t *attr, void *(*start_routine) (void *),
	void *arg)
{
	int rc;

	rc = pthread_create(thread, attr, start_routine, arg);
	if (rc)
		fatal("%s thread:");
}

int main(int argc, char *argv[])
{
	pthread_t timer_thread;
	pthread_t creator_thread;
	pthread_t unlinker_thread;
	pthread_t syncer_thread;

	Option.file_size = MAX_FILE_SIZE;
	Option.sleep_secs = 1;

	punyopt(argc, argv, myopt, "n:");
	setup();

	epthread_create("timer", &timer_thread, NULL, timer, NULL);
	epthread_create("creator", &creator_thread, NULL, creator, NULL);
	epthread_create("unlinker", &unlinker_thread, NULL, unlinker, NULL);
	epthread_create("syncer", &syncer_thread, NULL, syncer, NULL);

	pthread_join(syncer_thread, NULL);
	pthread_join(creator_thread, NULL);
	pthread_join(unlinker_thread, NULL);
	pthread_join(timer_thread, NULL);

	cleanup();
	return 0;
}
