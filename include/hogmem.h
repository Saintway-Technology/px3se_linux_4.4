/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef HOGMEM_H
#define HOGMEM_H

#include <style.h>

u64 memfree(void);
u64 memtotal(void);
void hog_memory(u64 numbytes);
void hog_leave_memory(u64 numbytes);
void hog_free(void);
void drop_caches(void);

#endif
