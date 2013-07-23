/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Miscellaneous utilities for writing tests.
 */


#include <fcntl.h>
#include <string.h>

#include <esys.h>
#include <twister.h>
#include <util.h>

/*
 * gen_name: generate a random file name of the given length.
 * The len include the null at the end.
 */
void gen_name(char *name, int len)
{
	static char file_name_char[] =	"abcdefghijklmnopqrstuvwxyz"
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"_0123456789";
	char *c = name;

	for (--len; len > 0; len--)
		*c++ = file_name_char[twister_urand(sizeof(file_name_char) - 1)];
	*c = '\0';
}


/*
 * crash crashes the system by causing a panic:
 * echo PANIC >/sys/kernel/debug/provoke-crash/DIRECT
 * echo panic >/proc/breakme
 */

void crash(void)
{
	char *panic = "PANIC";
	int fd = open("/sys/kernel/debug/provoke-crash/DIRECT", O_WRONLY);

	if (fd == -1) {
		fd = eopen("/proc/breakme", O_WRONLY);
		panic = "panic";
	}
	ewrite(fd, panic, strlen(panic));
	eclose(fd);
}
