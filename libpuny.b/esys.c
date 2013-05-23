/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * esys wraps all the system calls and generates fatal errors.
 * Some arguments are hard coded.
 */

#define _GNU_SOURCE

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <eprintf.h>
#include <esys.h>

void eclose(int fd)
{
	int rc = close(fd);
	if (rc == -1)
		fatal("close:");
}

int ecreat(const char *pathname)
{
	int fd = creat(pathname, 0666);
	if (fd == -1)
		fatal("creat %s:", pathname);
	return fd;
}

void efsync(int fd)
{
	int rc = fsync(fd);
	if (rc == -1)
		fatal("fsync:");
}

void efdatasync(int fd)
{
	int rc = fdatasync(fd);
	if (rc == -1)
		fatal("fdatasync:");
}

int eopen(const char *pathname, int flags)
{
	int fd = open(pathname, flags, 0666);
	if (fd == -1)
		fatal("open %s:", pathname);
	return fd;
}

int eread(int fd, void *buf, size_t nbyte)
{
	ssize_t	rc = read(fd, buf, nbyte);
	if (rc == -1)
		fatal("read:");
	return rc;
}

int epread(int fd, void *buf, size_t nbyte, off_t offset)
{
	ssize_t	rc = pread(fd, buf, nbyte, offset);
	if (rc == -1)
		fatal("pread:");
	return rc;
}

void esync(void)
{
	sync();
}

void esyncfs(int fd)
{
	syncfs(fd);
}

void eunlink(const char *pathname)
{
	int rc = unlink(pathname);
	if (rc == -1)
		fatal("unlink %s:", pathname);
}

int ewrite(int fildes, const void *buf, size_t nbyte)
{
	ssize_t	rc = write(fildes, buf, nbyte);
	if (rc == -1)
		fatal("write:");
	return rc;
}

int epwrite(int fd, void *buf, size_t nbyte, off_t offset)
{
	ssize_t	rc;

	rc = pwrite(fd, buf, nbyte, offset);
	if (rc == -1)
		fatal("pwrite:");
	return rc;
}

void *emmap(void *addr, size_t length, int prot, int flags,
	int fd, off_t offset)
{
	void *map = mmap(addr, length, prot, flags, fd, offset);
	if (map == MAP_FAILED)
		fatal("mmap:");
	return map;
}

void emunmap(void *addr, size_t length)
{
	int rc = munmap(addr, length);
	if (rc == -1)
		fatal("munmap:");
}

void emsync(void *addr, size_t length, int flags)
{
	int rc = msync(addr, length, flags);
	if (rc == -1)
		fatal("msync;");
}

