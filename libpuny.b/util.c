/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Miscellaneous utilities for writing tests.
 */


#include <fcntl.h>

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
 * echo panic >/proc/breakme
 */

void crash(void)
{
	static char panic[] = "panic";
	int fd = eopen("/proc/breakme", O_WRONLY);
	ewrite(fd, panic, sizeof(panic));
}
