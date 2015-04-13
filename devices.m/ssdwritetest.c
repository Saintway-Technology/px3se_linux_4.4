/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * ssdwritetest --- tool to measure the impact of writing to an SSD starting
 * with a factory-fresh disk and never freeing (trimming) any parts of it.
 *
 * This program writes blocks of data between offsets 0 and kDefaultSsdCapacity
 * on a storage device.  The blocks written cover the entire address range.
 * Each storage block is written once in a random order.  The program outputs
 * the time (in microseconds) taken by each write operation.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

typedef uint64_t u64;
typedef uint32_t u32;

/* The capacity of the (part of the) device to be tested.  Keep this number
 * smaller or equal to the size of the device (or partition) to be tested.
 */
const u64 kDefaultSsdCapacity = 14ULL * 1024 * 1024 * 1024;

/* The size of write operations to the device. */
const u64 kDefaultBlockSize = 65536 * 4;

/* The maximum practical number of blocks.  Feel free to increase.  Note that
 * this program allocates an array with size equal to (number of blocks) * 4.
 */
const int kMaxNBlocks = 128 * 1024 * 1024;

/* Log in base 2 of the interval at which progress reports are printed. */
static int report_period_logarithm;

/* The wall clock time in us (microseconds) when the process starts. */
static u64 process_epoch_us;

/* The default path for the program output. */
const char *default_output_name = "/tmp/ssd_write_test_output";

FILE *output_file;

/* Returns the wall clock in microseconds. */
u64 gettimeofday_us(void) {
  struct timeval time;
  gettimeofday(&time, NULL);
  return time.tv_sec * 1000000LL + time.tv_usec;
}


static void ERROR(const char *format, ...) {
  va_list ap;

  va_start(ap, format);
  fprintf(stderr, "ERROR: ");
  vfprintf(stdout, format, ap);
  if (errno != 0) {
    fprintf(stdout, " (%s)", strerror(errno));
  }
  fprintf(stdout, "\n");
  exit(1);
}


static void LOG(const char *format, ...) {
  va_list ap;
  u64 time = gettimeofday_us() - process_epoch_us;

  va_start(ap, format);
  fprintf(stdout, "LOG: %d.%06d ",
          (int) (time / 1000000),
          (int) (time % 1000000));
  vfprintf(stdout, format, ap);
  fprintf(stdout, "\n");
}


static void assert(int condition, char *failure_message) {
  if (!condition) {
    ERROR(failure_message);
  }
}


/* Allocates memory and aborts if the allocation fails. */
static void *safe_malloc(int size) {
  void *p = malloc(size);
  assert(p != NULL, "out of memory");
  return p;
}


/* Initializes A with a random permutation of 0, ..., N-1 using the
 * Fisher-Yates-Knuth's algorithm. (Proof by induction on N.)
 */
void set_to_random_identity_permutation(int *a, int n) {
  int i, j;
  a[0] = 0;
  for (i = 1; i < n - 1; i++) {
    j = (u32) (random() * (u64) i / RAND_MAX);
    a[i] = a[j];
    a[j] = i;
  }
}


void reset_progress_report(void) {
  report_period_logarithm = 0;
}


/* Reports progress with exponential throttling, i.e. produces the ith report
 * only when i is a power of 2.
 */
void report_progress(int i, int n) {
  if (i >= (1 << report_period_logarithm)) {
    LOG("completed %d out of %d ops", i, n);
    report_period_logarithm += 1;
  }
}


/* Produces one sample of output. */
void output_sample(int x) {
  if (fprintf(output_file, "%d\n", x) < 0) {
    ERROR("output_sample failed in fprintf");
  }
}


/* Writes data all over the device associated with file descriptors FD.  The
 * data is written as N_BLOCKS, each with size BLOCK_SIZE (in bytes), at the
 * addresses specified by BLOCK_ADDRESSES (as byte offsets).  Outputs the time
 * (in microseconds) taken to write each block.
 */
void write_blocks(int fd, int *block_addresses, int n_blocks, int block_size) {
  int i;
  int *buffer;
  u64 time_before, time_after;

  int result = posix_memalign((void **) &buffer, block_size, block_size);
  if (result != 0) {
    errno = result;  /* errno is not set by posix_memalign */
    ERROR("cannot allocate %d bytes of aligned memory", block_size);
  }

  for (i = 0; i < n_blocks; i++) {

    /* Seek to shuffled location */
    off64_t offset = block_addresses[i] * (u64) block_size;
    off64_t result = lseek64(fd, offset, SEEK_SET);
    if (result != offset) {
      ERROR("cannot seek to %lld", offset);
    }

    /* Ensure that the disk doesn't optimize the write. */
    buffer[0] = random();

    time_before = gettimeofday_us();
    /* Write the buffer */
    int written = write(fd, buffer, block_size);
    time_after = gettimeofday_us();

    /* Check for failure */
    if (written != block_size) {
      ERROR("write block %d at offset %lld, written = %d", i, offset, written);
    }

    u64 t = time_after - time_before;
    assert(t < (1ULL << 32), "time overflow");
    output_sample((u32) t);
    report_progress(i, n_blocks);
  }

  free(buffer);
}


/* Runs the test on the device specified by SSD_NAME.  SSD_CAPACITY is the
 * device capacity (in bytes), and BLOCK_SIZE is the requested size of each
 * write.  Currently we run two full and identical passes over the device.
 */
void run(const char *ssd_name,
         u64 ssd_capacity,
         int block_size) {
  int fd;

  int n_blocks = (u32) (ssd_capacity / block_size);
  assert(n_blocks * (u64) block_size == ssd_capacity, "bad block size");
  assert(n_blocks <= kMaxNBlocks, "too many blocks");
  int *block_addresses = (int *) safe_malloc(n_blocks * sizeof(int));

  LOG("computing random permutation for %d blocks", n_blocks);
  set_to_random_identity_permutation(block_addresses, n_blocks);

  fd = open(ssd_name, O_WRONLY | O_DIRECT);
  if (fd < 0) {
    ERROR("cannot open %s", ssd_name);
  }

  LOG("writing blocks (pass 1)");
  reset_progress_report();
  write_blocks(fd, block_addresses, n_blocks, block_size);

  LOG("writing blocks (pass 2)");
  reset_progress_report();
  write_blocks(fd, block_addresses, n_blocks, block_size);
}


void usage(void) {
  fprintf(stderr, "usage: ssdwritetest <device> [<output file>]\n");
  fprintf(stderr, "warning: this program destroys the content of <device>\n");
  exit(1);
}


int main(int ac, char **av) {
  u64 ssd_capacity = kDefaultSsdCapacity;
  int block_size = kDefaultBlockSize;
  const char *ssd_name = NULL;
  const char *output_name = default_output_name;

  process_epoch_us = gettimeofday_us();

  if (ac >= 2) {
    ssd_name = av[1];
    if (ac == 3) {
      output_name = av[2];
    }
  } else {
    usage();
  }

  srandom(1);

  LOG("ssd = %s, output = %s", ssd_name, output_name);
  output_file = fopen(output_name, "w");
  if (output_file == NULL) {
    ERROR("cannot open %s", output_file);
  }
  run(ssd_name, ssd_capacity, block_size);
  return 0;
}
