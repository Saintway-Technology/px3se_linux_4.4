/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * esys wraps all the system calls and calls fatal on errors.
 */

void eclose(int fd);
void echdir(const char *pathname);
int ecreat(const char *pathname);
void efsync(int fd);
void efdatasync(int fd);
void emkdir(const char *pathname);
int eopen(const char *pathname, int flags);
int eread(int fd, void *buf, size_t nbyte);
int epread(int fd, void *buf, size_t nbyte, off_t offset);
void esync(void);
void esyncfs(int fd);
void eunlink(const char *pathname);
int ewrite(int fd, const void *buf, size_t nbyte);
int epwrite(int fd, void *buf, size_t nbyte, off_t offset);
void *emmap(void *addr, size_t length, int prot, int flags,
	int fd, off_t offset);
void emunmap(void *addr, size_t length);
void emsync(void *addr, size_t length, int flags);
