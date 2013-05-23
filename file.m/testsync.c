/*
 * Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * These tests verify that sync works correctly.
 *
 * Each test is written in two phases. The first sets up the test,
 * writes the data, calls sync and then crashes the system.
 *
 * After the system reboots, the second phase of the test verifies
 * the results and cleans up.
 *
 * Current phases:
 * writer - writes to the given file, calls fsync, then crashes.
 * mapper - uses memory mapped file to write data, calls msync,
 *          then crashes.
 * verifier - verifies that all the data that was synced is there.
 *
 * Uses a 64 bit counter for the data.
 */

#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <debug.h>
#include <eprintf.h>
#include <esys.h>
#include <puny.h>
#include <style.h>
#include <util.h>

enum {
	CHUNK_SIZE = (1<<13),
	NUM_U64_PER_CHUNK = CHUNK_SIZE / sizeof(u64)
};

static char *PhaseName;
typedef void (*phasefn_t)(void);

struct phase {
	char *name;
	phasefn_t fn;
};

u64 Buf[NUM_U64_PER_CHUNK];
u64 Value;

static void fill(void)
{
	int i;

	for (i = 0; i < NUM_U64_PER_CHUNK; i++)
		Buf[i] = Value++;
}

static void verify(void)
{
	int i;

	for (i = 0; i < NUM_U64_PER_CHUNK; i++) {
		if (Buf[i] != Value)
			fatal("Expected %lld is %lld", Value, Buf[i]);
		Value++;
	}
}

static void writer(void)
{
	u64 size = Option.file_size;
	u64 chunks = size / sizeof(Buf);
	u64 i;
	int fd = eopen(Option.file, O_WRONLY | O_CREAT | O_TRUNC);

	Value = 0;
	for (i = 0; i < chunks; i++) {
		fill();
		ewrite(fd, Buf, sizeof(Buf));
	}
	efsync(fd);
	crash();
}

static void mapper(void)
{
	u64 size = Option.file_size;
	u64 chunks = size / sizeof(Buf);
	u64 i;
	u8 *map;
	u8 *m;
	int fd = eopen(Option.file, O_RDWR | O_CREAT | O_TRUNC);

	Value = 0;
	epwrite(fd, "", 1, size - 1);
	map = emmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	for (m = map, i = 0; i < chunks; i++, m += sizeof(Buf)) {
		fill();
		memcpy(m, Buf, sizeof(Buf));
	}
	emsync(map, size, MS_SYNC);
	crash();
}

static void verifier(void)
{
	u64 size = Option.file_size;
	u64 blocks = size / sizeof(Buf);
	u64 i;
	int fd = eopen(Option.file, O_RDONLY);

	Value = 0;
	for (i = 0; i < blocks; i++) {
		eread(fd, Buf, sizeof(Buf));
		verify();
	}
	eclose(fd);
}

static struct phase Phase[] = {
	{ "writer", writer },
	{ "mapper", mapper },
	{ "verifier", verifier }
};

static phasefn_t find_phase(char *name)
{
	int n = sizeof(Phase) / sizeof(struct phase);
	int i;

	for (i = 0; i < n; i++)
		if (strcmp(name, Phase[i].name) == 0)
			return Phase[i].fn;
	fatal("Phase %s: not found", name);
	return NULL;
}

static bool myopt(int c)
{
	switch (c) {
	case 'p':
		PhaseName = strdup(optarg);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void usage (void)
{
	pr_usage("-f <file> -p <test phase name>\n"
		"\tf - file to write");
}

int main(int argc, char *argv[])
{
	phasefn_t fn;

	punyopt(argc, argv, myopt, "p:");
	fn = find_phase(PhaseName);
	if (fn)
		fn();
	return 0;
}
