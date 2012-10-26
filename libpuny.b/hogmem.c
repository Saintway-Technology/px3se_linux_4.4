/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <debug.h>
#include <eprintf.h>
#include <style.h>
#include <twister.h>

static void *Hog;
static u64 Numbytes;
static bool Init_rand = FALSE;

static void fill_rand(u64 *hog, u64 numbytes)
{
	u64 n = numbytes / sizeof(u64);
	u64 r = numbytes % sizeof(u64);
	u64 i;
	u8 *piglet;

	for (i = 0; i < n; i++)
		hog[i] = twister_random();

	piglet = (u8 *)&hog[i];
	for (i = 0; i < r; i++)
		piglet[i] = twister_random();
}

/*
 * Returns total bytes of memory in system.
 */
u64 memtotal(void)
{
	u64 memtotal;
	int rc;
	FILE *fp = fopen("/proc/meminfo", "r");

	if (!fp)
		fatal("fopen /proc/meminfo :");
	rc = fscanf(fp, "MemTotal:%llu kB\n", &memtotal);
	if (rc != 1)
		fatal("Didn't read MemTotal:");
	fclose(fp);
	return memtotal * KIBI;
}

/*
 * Drops pagecache, dentries and inodes that
 * are not in use. Should be called at beginning
 * of test.
 */
void drop_caches(void)
{
	int rc;
	FILE *fp;

	sync();
	fp = fopen("/proc/sys/vm/drop_caches", "w");
	if (!fp) {
		warn("fopen /proc/sys/vm/drop_caches :");
		return;
	}
	rc = fprintf(fp, "3\n");
	if (rc == -1)
		fatal("drop_caches failed");
	fclose(fp);
}

/*
 * Returns number of free bytes of memory in system.
 */
u64 memfree(void)
{
	u64 ignore;
	u64 memfree;
	int rc;
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");
	if (!fp)
		fatal("fopen /proc/meminfo :");
	rc = fscanf(fp, "MemTotal:%llu kB\n", &ignore);
	if (rc != 1)
		fatal("Didn't read MemTotal:");
	rc = fscanf(fp, "MemFree:%llu kB\n", &memfree);
	if (rc != 1)
		fatal("Didn't read MemFree:");
	fclose(fp);
	return memfree * 1024;
}

/*
 * Hog numbytes of memory. It is initialized and locked in.
 */
void hog_memory(u64 numbytes)
{
	int rc;

	Hog = malloc(numbytes);
	if (!Hog)
		fatal("Couldn't hog %llu bytes of memory", numbytes);
	Numbytes = numbytes;
	if (!Init_rand) {
		twister_random_seed();
		Init_rand = TRUE;
	}
	fill_rand(Hog, numbytes);
	rc = mlock(Hog, numbytes);
	if (rc)
		fatal("Couldn't lock memory:");
}

/*
 * Leave the give number of bytes free
 */
void hog_leave_memory(u64 numbytes)
{
	u64 mem = memfree();

	if (mem <= numbytes)
		return;
	hog_memory(mem - numbytes);
}

/*
 * Frees memory allocated to hog but doesn't free it
 * back to the system.
 */
void hog_free(void)
{
	munlock(Hog, Numbytes);
	free(Hog);
	Hog = NULL;
	Numbytes = 0;
}
