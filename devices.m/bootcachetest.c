/* Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
/*
 * Tests for the boot cache
 * 1. Uses a file with a loopback device
 * 2. Mounts a file system on loopback device
 * 3. Copy files to it
 * 4. Unmount file system
 * 5. Create dm-bootcache device and add to loopback device
 * 6. Access some blocks
 * 7. Generate boot cache
 * 8. Verify block map trace
 * 9. Mark free
 * 10. Reset bootcache
 * 11. Should be marked valid
 * 12. Short pause to allow bootcache to fill cache
 * 13. Read same blocks
 * 14. Verify content
 * 15. Cleanup
 *
 * Bad Mapping
 * 1-6 Same
 * 7. While generating bootcache corrupt
 *    one of the trace values - make very big
 * 8. Write out corrupt trace
 * 9-15 Clean
 * Should succeed - just bypasses cache for that read.
 *
 * Corrupt Header so it tries to read beyond end of device.
 * 1-6 Same
 * 7. While generating bootcache corrupt
 *    the amount of meta data so it extends beyond partition
 * 8. Write out corrupt header
 * 9-15 Clean
 * Should succeed - marks cache as invalid.
 *
 * Trash data
 * The bootcache doesn't detect that - it relies on verity
 * for that which escalates to recovery.
 *
 */

#define _XOPEN_SOURCE 600
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <debug.h>
#include <eprintf.h>
#include <puny.h>
#include <style.h>
#include <twister.h>

#include <dm-bootcache.h>

enum {	MAX_CMD = 1024,
	MAX_PATH = 512,
	MAX_CHUNKS = 128,
	BLOCK_SIZE = 4096,
	BLOCK_SIZE_64 = BLOCK_SIZE / sizeof(u64),
	SECTOR_SIZE = 512,
	NUM_BLOCKS = 10000,
	DEV_FILE_SIZE = NUM_BLOCKS * BLOCK_SIZE,
	FS_SIZE = DEV_FILE_SIZE / 2};

struct Block_range {
	u64 blocknum;
	u64 numblocks;
};

static char Dev_file[] = "/usr/local/xyzzy";
static char Dev_mapper[] = "bootcache-testdev";
static char *Loopdev;

static struct bootcache_hdr Header;
static struct {
	struct bootcache_trace *tr;
	int num;
} Trace;

static char Valid_file[MAX_PATH];
static char Free_file[MAX_PATH];
static char Header_file[MAX_PATH];
static char Blocktrace_file[MAX_PATH];

static u64 Trace_start;
static u64 Cache_start;
static u64 Chunk_size;
static u64 Sectors_per_chunk;

static struct bootcache_args {
	u64 start;
	u64 end;
	u64 cache_start;
	u64 size_limit;
	u64 max_trace;
	u64 max_pages;
	char signature[MAX_SIGNATURE];
} Bootcache_args = {
	.start = 0,
	.end = DEV_FILE_SIZE / SECTOR_SIZE,
	.cache_start = DEV_FILE_SIZE / SECTOR_SIZE / 2,
	.size_limit = 512,
	.max_trace = 2056,
	.max_pages = 10000,
	.signature = "10efc878943ab8e6cf"

};

struct Block_range Block_test[] = {
	{50, 3},
	{100, 7},
	{0, 1},
	{1000, 16},
	{500, 8},
	{0, 0}	/* End marker */
};

/*
 * Some platforms automatically read additional blocks other
 * than those specifically requested by the test. Only traces
 * marked required must match.
 */
struct {
	struct bootcache_trace tr;
	bool is_required;
} Expected_trace[] = {
	/*
	 * This first set of reads were done by the system looking
	 * for partition tables and other information.
	 * It may change in the future.
	 * All reads come down to the boot cache layer from
	 * user space in 8 sector reads so can't test varying
	 * the size of the reads.
	 */
	{{79872, 8, 0}, 0},
	{{79984, 8, 0}, 0},
	{{0, 8, 0}, 0},
	{{8, 8, 0}, 0},
	{{79992, 8, 0}, 0},
	{{79736, 8, 0}, 0},
	{{79936, 8, 0}, 0},
	{{79744, 8, 0}, 0},
	{{79600, 8, 0}, 0},
	{{2048, 8, 0}, 0},
	{{24, 8, 0}, 0},
	{{56, 8, 0}, 0},
	{{120, 8, 0}, 0},
	{{16, 8, 0}, 0},
	{{128, 8, 0}, 0},
	{{64, 8, 0}, 0},
	{{512, 8, 0}, 0},
	{{32, 8, 0}, 0},
	{{4096, 8, 0}, 0},
	/* {50, 3} */
	{{400, 8, 0}, 0},
	{{408, 8, 0}, 0},
	{{416, 8, 0}, 0},
	/* {100, 7} */
	{{800, 8, 0}, 0},
	{{808, 8, 0}, 0},
	{{816, 8, 0}, 0},
	{{824, 8, 0}, 0},
	{{832, 8, 0}, 0},
	{{840, 8, 0}, 0},
	{{848, 8, 0}, 0},
	/* {0, 1} Note attempt at read ahead */
	{{0, 8, 0}, 1},
	{{8, 8, 0}, 0},
	{{16, 8, 0}, 0},
	{{24, 8, 0}, 0},
	/* {1000, 16} */
	{{8000, 8, 0}, 1},
	{{8008, 8, 0}, 1},
	{{8016, 8, 0}, 1},
	{{8024, 8, 0}, 1},
	{{8032, 8, 0}, 1},
	{{8040, 8, 0}, 1},
	{{8048, 8, 0}, 1},
	{{8056, 8, 0}, 1},
	{{8064, 8, 0}, 1},
	{{8072, 8, 0}, 1},
	{{8080, 8, 0}, 1},
	{{8088, 8, 0}, 1},
	{{8096, 8, 0}, 1},
	{{8104, 8, 0}, 1},
	{{8112, 8, 0}, 1},
	{{8120, 8, 0}, 1},
	/* {500, 8} */
	{{4000, 8, 0}, 1},
	{{4008, 8, 0}, 1},
	{{4016, 8, 0}, 1},
	{{4024, 8, 0}, 1},
	{{4032, 8, 0}, 1},
	{{4040, 8, 0}, 1},
	{{4048, 8, 0}, 1},
	{{4056, 8, 0}, 1},
	{{4064, 8, 0}, 0},
	{{4072, 8, 0}, 0},
	{{4080, 8, 0}, 0},
	{{4088, 8, 0}, 0},
	{{4096, 8, 0}, 0},
	{{4104, 8, 0}, 0},
	{{4112, 8, 0}, 0},
	{{4120, 8, 0}, 0},
	{{0, 0, 0}, 0}
};

static int eopen(const char *file, int flags)
{
	int fd;

	fd = open(file, flags);
	if (fd == -1)
		fatal("open %s:", file);
	return fd;
}

static int efsync(int fd)
{
	int rc;

	rc = fsync(fd);
	if (rc == -1)
		fatal("fsync:");
	return rc;
}

static int eclose(int fd)
{
	int rc;

	rc = close(fd);
	if (rc == -1)
		fatal("close:");
	return rc;
}

static void gen_file_name(char *file_name, int size, const char *fmt,
				const char *prefix, const char *name)
{
	int rc;

	rc = snprintf(file_name, size, fmt, prefix, name);
	if (rc >= size)
		fatal("Name too long %s", name);
}

/* find the dm-[0-9] name for bootcache device mapper */
static char *get_dm(const char *dev_mapper)
{
	char path[MAX_PATH];
	char symlink[MAX_PATH];
	ssize_t rc;

	snprintf(path, sizeof(path), "/dev/mapper/%s", dev_mapper);
	rc = readlink(path, symlink, sizeof(symlink)-1);
	if (rc == -1)
		fatal("readlink %s:", path);
	symlink[rc] = '\0';
	/* Skip for the first three characters: "../" */
	return estrdup(&symlink[3]);
}

/*
 * Writing '1' to the free file indicates to
 * the bootcache that it can free all of its
 * resources.
 */
static void free_bootcache(void)
{
	char buf[] = "1";
	int fd;
	int rc;

	fd = eopen(Free_file, O_WRONLY);
	rc = write(fd, buf, 1);
	if (rc == -1) {
		fatal("write %s:", Free_file);
	}
	eclose(fd);
}

/*
 * A '1' in the first byte of the valid file, indicates, the
 * cache is valid. Otherwise is should be '0';
 */
static bool is_valid(void)
{
	char buf[1];
	int fd;
	int rc;

	fd = eopen(Valid_file, O_RDONLY);
	rc = read(fd, buf, sizeof(buf));
	eclose(fd);
	if ((rc == -1) || (rc == 0)) {
		fatal("read %s:", Valid_file);
	}
	return buf[0] == '1';
}

static void gen_file_names(const char *device_mapper)
{
	const char fmt[] = "/sys/devices/virtual/block/%s/dm/%s";
	char *dm_name = get_dm(device_mapper);

	gen_file_name(Valid_file, sizeof(Valid_file),
			fmt, dm_name, "valid");
	gen_file_name(Free_file, sizeof(Free_file),
			fmt, dm_name, "free");
	gen_file_name(Header_file, sizeof(Header_file),
			fmt, dm_name, "header");
	gen_file_name(Blocktrace_file, sizeof(Blocktrace_file),
			fmt, dm_name, "blocktrace");
	free(dm_name);
}

static void read_header(void)
{
	int fd;
	int rc;

	fd = eopen(Header_file, O_RDONLY);
	rc = read(fd, &Header, sizeof(Header));
	if (rc == -1)
		fatal("read %s:", Header_file);
	eclose(fd);
	if (Header.magic != BOOTCACHE_MAGIC)
		fatal("Bad magic %u != %u", Header.magic, BOOTCACHE_MAGIC);
	if (Header.version != BOOTCACHE_VERSION)
		fatal("Bad version %u != %u", Header.version, BOOTCACHE_VERSION);
	Chunk_size = Header.alignment;
	Sectors_per_chunk = Chunk_size / SECTOR_SIZE;
}

/*
 * get_file_size - Because we are reading a pseudo file in sysfs,
 * we scan it to see how big it is.
 */
static u64 get_file_size(const char *file)
{
	char buf[Chunk_size];
	ssize_t rc;
	u64 sum = 0;

	int fd = eopen(file, O_RDONLY);
	for (;;) {
		rc = read(fd, buf, sizeof(buf));
		if (rc == -1)
			fatal("read %s:", file);
		if (rc == 0)
			break;
		sum += rc;
	}
	eclose(fd);
	return sum;
}

static void dump_trace(void)
{
	struct bootcache_trace *tr = Trace.tr;
	int i;

	if (0)
		for (i = 0; i < Trace.num; i++, tr++)
			printf("%llu %llu %llu\n", tr->sector, tr->count, tr->ino);
}

static void read_trace(void)
{
	/*
	 * Because this is a sysfs file, we have to read it to get
	 * its size. Even if more data is appended to the file, we
	 * don't care, we just want the data up to this point in
	 * time.
	 */
	u64 n = get_file_size(Blocktrace_file);
	ssize_t rc;
	int fd;
	char *b;

	Trace.tr  = emalloc(n);
	Trace.num = n / sizeof(struct bootcache_trace);
	fd = eopen(Blocktrace_file, O_RDONLY);
	/*
	 * Because sysfs only returns a page at a time,
	 * will need to do the read in a loop.
	 */
	for (b = (char *)Trace.tr; n; n -= rc, b += rc) {
		rc = read(fd, b, n);
		if (rc == -1)
			fatal("read %s:", Blocktrace_file);
		if (rc == 0)
			fatal("trying to read %lld bytes", n);
	}
	dump_trace();
	eclose(fd);
}

static u64 num_meta_sectors(void)
{
	u64 num_bytes = Trace.num * sizeof(*Trace.tr);

	/* Align to page boundary then convert to sectors */
	return ((num_bytes + Chunk_size - 1) / Chunk_size) * Sectors_per_chunk;
}

static u64 num_sectors_in_cache(void)
{
	int i;
	u64 sum = 0;

	for (i = 0; i < Trace.num; i++)
		sum += Trace.tr[i].count;
	return sum;
}

static void compute_sections(void)
{
	Header.num_trace_recs = Trace.num;
	Header.sectors_meta = num_meta_sectors();
	Header.sectors_data = num_sectors_in_cache();
	Trace_start = Header.sector + Sectors_per_chunk;
	Cache_start = Trace_start + Header.sectors_meta;
}

static void *malloc_buf(size_t nchunks)
{
	void *buf;
	int rc;

	rc = posix_memalign(&buf, BLOCK_SIZE, nchunks * BLOCK_SIZE);
	if (rc)
		fatal("posix_memalign:", rc);
	return buf;
}

static void copy_trace(int dst, int src, struct bootcache_trace tr, void *buf)
{
	u64 n;
	u64 remainder;
	u64 offset;
	int rc;

	offset = tr.sector * SECTOR_SIZE;
	remainder = tr.count * SECTOR_SIZE;
	n = MAX_CHUNKS * Chunk_size;
	while (remainder) {
		if (n > remainder)
			n = remainder;
		rc = pread(src, buf, n, offset);
		if (rc < 0)
			fatal("pread trace offset=%llu num sectors=%llu:",
				offset / SECTOR_SIZE, n / SECTOR_SIZE);
		if (rc != n)
			fatal("pread read only %u bytes expected %llu",
				rc, n);
		rc = write(dst, buf, n);
		if (rc < 0)
			fatal("write trace offset=%llu num sectors=%llu:",
				offset / SECTOR_SIZE, n / SECTOR_SIZE);
		if (rc != n)
			fatal("write wrote only %u bytes expected %llu",
				rc, n);
		offset += n;
		remainder -= n;
	}
}

static void copy_blocks(const char *device)
{
	int i;
	off_t rc;

	int src = eopen(device, O_RDONLY);
	int dst = eopen(device, O_WRONLY);
	void *buf = malloc_buf(MAX_CHUNKS);

	rc = lseek(dst, Cache_start * SECTOR_SIZE, SEEK_SET);
	if (rc == -1)
		fatal("lseek for cache start:");
	for (i = 0; i < Trace.num; i++)
		copy_trace(dst, src, Trace.tr[i], buf);
	free(buf);
	efsync(dst);
	eclose(dst);
	eclose(src);
}

static void write_trace(const char *file)
{
	int fd;
	ssize_t rc;
	ssize_t size = Trace.num * sizeof(*Trace.tr);

	fd = eopen(file, O_WRONLY);
	rc = pwrite(fd, Trace.tr, size, Trace_start * SECTOR_SIZE);
	if (rc != size)
		fatal("pwrite %s rc=%ld size=%ld:", file, rc, size);
	efsync(fd);
	eclose(fd);
}

/*
 * The header is written last after everything else, cache data and traces,
 * have been written to the disk. The header is what tells the boot cache
 * on the next boot that the cache is valid and should be used.
 * For correctness, we don't have to flush the header but the default
 * flush time is 10 minutes and there is no reason to wait.
 */
static void write_header(const char *file)
{
	int fd;
	int rc;

	fd = eopen(file, O_WRONLY);
	rc = pwrite(fd, &Header, sizeof(Header), Header.sector * SECTOR_SIZE);
	if (rc != sizeof(Header))
		fatal("pwrite %s rc=%d:", file, rc);
	efsync(fd);
	eclose(fd);
}

static void build_bootcache(char *dev_loop, char *dev_mapper)
{
	gen_file_names(dev_mapper);
	read_header();
	read_trace();
	compute_sections();
	copy_blocks(dev_loop);
	write_trace(dev_loop);
	write_header(dev_loop);
}

static void corrupt_bootcache(char *dev_loop, char *dev_mapper)
{
	struct bootcache_trace *t;

	gen_file_names(dev_mapper);
	read_header();
	read_trace();
	compute_sections();
	copy_blocks(dev_loop);
	/*
	 * Corrupt trace before writing it back out
	 */
	t = &Trace.tr[Trace.num / 2];
	t->sector = 0xbadc0dedeadLL;

	write_trace(dev_loop);
	write_header(dev_loop);
}

static void corrupt_header(char *dev_loop, char *dev_mapper)
{
	gen_file_names(dev_mapper);
	read_header();
	read_trace();
	compute_sections();
	copy_blocks(dev_loop);
	write_trace(dev_loop);
	/*
	 * Corrupt header before writing it back out.
	 */
	Header.sectors_meta = Option.file_size / SECTOR_SIZE;

	write_header(dev_loop);
}

static void run(char *cmd)
{
	int rc;

	fprintf(stderr, "|%s\n", cmd);
	rc = system(cmd);
	if (rc)
		fatal("%s:%d:", cmd, rc);
}

/*
 * To verify contents of blocks, test fills block with random
 * numbers seeded by the block number.
 */
static void fill_block(int fd, u64 blocknum)
{
	u64 buf[BLOCK_SIZE_64];
	int i;
	int rc;

	init_twister(blocknum);
	for (i = 0; i < BLOCK_SIZE_64; i++)
		buf[i] = twister_random();
	rc = pwrite(fd, buf, BLOCK_SIZE, blocknum * BLOCK_SIZE);
	if (rc != BLOCK_SIZE) {
		if (rc == -1)
			fatal("pwrite:");
		fatal("Tried to write %d but wrote %d", BLOCK_SIZE, rc);
	}
}

static void fill(int fd, u64 numblocks)
{
	u64 i;

	for (i = 0; i < numblocks; i++)
		fill_block(fd, i);
}

static void test_block(void *buf, u64 blocknum)
{
	u64 *b = buf;
	u64 t;
	u64 i;

	init_twister(blocknum);
	for (i = 0; i < BLOCK_SIZE_64; i++, b++) {
		t = twister_random();
		if (*b != t)
			fatal("content doesn't match for block=%llu at"
				" offset=%llu %llu!=%llu",
				blocknum, i, *b, t);
	}
}

static void test_blocks(u8 *buf, u64 blocknum, u64 numblocks)
{
	u64 i;

	for (i = 0; i < numblocks; i++)
		test_block(&buf[i * BLOCK_SIZE], blocknum + i);
}

static void create_device(char *name)
{
	int fd;

	fd = creat(name, 0666);
	if (fd == -1)
		fatal("creat %s:", name);
	fill(fd, Option.file_size / BLOCK_SIZE);
	close(fd);
}

static char *get_loopback_device(void)
{
	FILE *fp;
	char path[MAX_PATH];
	char *p;
	int c;

	fp = popen("losetup -f", "r");
	if (!fp)
		fatal("popen \"losetup -f\":");
	for (p = path; p < &path[MAX_PATH - 1]; p++) {
		c = getc(fp);
		if (c == EOF) {
			if (p == path)
				fatal("nothing returned by \"losetup -f\"");
			break;
		}
		if (isspace(c))
			break;
		*p = c;
	}
	*p = '\0';
	pclose(fp);

	return estrdup(path);
}

static void init_loopback_device(char *path, char *device)
{
	char cmd[MAX_CMD];

	snprintf(cmd, sizeof(cmd), "losetup %s %s", path, device);
	run(cmd);
}

static void delete_loopback_device(char **path)
{
	struct timespec sleep = { 0, 1 * ONE_MILLION };
	char cmd[MAX_CMD];

	/*
	 * Wait for things to settle down
	 * before cleaning up
	 */
	nanosleep(&sleep, NULL);
	snprintf(cmd, sizeof(cmd), "losetup -d %s", *path);
	run(cmd);
	free(*path);
	*path = NULL;
}

/* Pass in a structure with all the fields */
static void create_dm_bootcache(struct bootcache_args *args,
	char *dev_loop, char *dev_mapper)
{
	char cmd[MAX_CMD];
	FILE *fp;
	int rc;

	snprintf(cmd, sizeof(cmd), "dmsetup create %s", dev_mapper);
	/*
	 * dmsetup <start> <end> bootcache <loop_dev> <cache_start>
	 *   <signature> <size_limit> <max_trace> <max_pages>
	 */
	fp = popen(cmd, "w");
	if (!fp)
		fatal("popen \"%s\":", cmd);
	fprintf(fp, "%llu %llu bootcache %s %llu %s %llu %llu %llu\n",
		args->start, args->end, dev_loop, args->cache_start,
		args->signature, args->size_limit, args->max_trace,
		args->max_pages);
	rc = pclose(fp);
	if (rc)
		fatal("dmsetup create %s %d", dev_mapper, rc);
}

static void remove_dm(char *dev_mapper)
{
	char cmd[MAX_CMD];

	snprintf(cmd, sizeof(cmd), "dmsetup remove %s", dev_mapper);
	run(cmd);
}

static void read_blocks(int fd, u64 blocknum, u64 numblocks)
{
	u64 offset = blocknum * BLOCK_SIZE;
	u64 size = numblocks * BLOCK_SIZE;
	u8 *buf;
	s64 rc;

	buf = malloc_buf(numblocks);
	rc = pread(fd, buf, size, offset);
	if (rc != size) {
		if (rc == -1)
			fatal("pread blocknum=%llu numblocks-%llu:",
				blocknum, numblocks);
		fatal("pread expected=%llu read=%ld",
			size, rc);
	}
	test_blocks(buf, blocknum, numblocks);
	free(buf);
}

static void read_block_ranges(char *dev_mapper)
{
	char path[MAX_PATH];
	struct Block_range *r;
	int fd;

	snprintf(path, sizeof(path), "/dev/mapper/%s", dev_mapper);
	fd = open(path, O_RDONLY);
	if (fd == -1)
		fatal("open %s:", path);
	for (r = Block_test; r->numblocks; r++)
		read_blocks(fd, r->blocknum, r->numblocks);
	close(fd);
}

static bool is_match (struct bootcache_trace *a, struct bootcache_trace *b)
{
	return (a->sector == b->sector) &&
		(a->count == b->count) &&
		(a->ino == b->ino);
}

static void check_trace(void)
{
	int i;

	for (i = 0; i < Trace.num && Expected_trace[i].tr.count; i++)
		if (!is_match(&Trace.tr[i], &Expected_trace[i].tr) &&
		    Expected_trace[i].is_required)
			fatal("traces not the same %d", i);
	if (Trace.num != i)
		fatal("Have more traces than expected %d > %i",
			Trace.num, i);
	for (; Expected_trace[i].tr.count; i++)
		if (Expected_trace[i].is_required)
			fatal("Required trace missing %d", i);
}

static void cleanup(void)
{
	if (!Option.cleanup)
		return;
	remove_dm(Dev_mapper);
	delete_loopback_device(&Loopdev);
	unlink(Option.file);
}

static void normal(void)
{
	create_device(Option.file);
	Loopdev = get_loopback_device();
	init_loopback_device(Loopdev, Option.file);
	create_dm_bootcache(&Bootcache_args, Loopdev, Dev_mapper);
	read_block_ranges(Dev_mapper);
	build_bootcache(Loopdev, Dev_mapper);
	check_trace();
	free_bootcache();

	/* Reset bootcache */
	remove_dm(Dev_mapper);
	create_dm_bootcache(&Bootcache_args, Loopdev, Dev_mapper);

	if (!is_valid())
		fatal("The boot cache should be valid");
	read_block_ranges(Dev_mapper);
	free_bootcache();

	cleanup();
}

static void bad_map(void)
{
	create_device(Option.file);
	Loopdev = get_loopback_device();
	init_loopback_device(Loopdev, Option.file);
	create_dm_bootcache(&Bootcache_args, Loopdev, Dev_mapper);
	read_block_ranges(Dev_mapper);
	corrupt_bootcache(Option.file, Dev_mapper);
	free_bootcache();

	/* Reset bootcache */
	remove_dm(Dev_mapper);
	create_dm_bootcache(&Bootcache_args, Loopdev, Dev_mapper);

	if (!is_valid())
		fatal("The boot cache should be valid");
	read_block_ranges(Dev_mapper);
	free_bootcache();

	cleanup();
}

static void meta_data_too_big(void)
{
	create_device(Option.file);
	Loopdev = get_loopback_device();
	init_loopback_device(Loopdev, Option.file);
	create_dm_bootcache(&Bootcache_args, Loopdev, Dev_mapper);
	read_block_ranges(Dev_mapper);
	corrupt_header(Option.file, Dev_mapper);
	free_bootcache();

	/* Reset bootcache */
	remove_dm(Dev_mapper);
	create_dm_bootcache(&Bootcache_args, Loopdev, Dev_mapper);

	if (is_valid())
		fatal("The boot cache should NOT be valid");
	read_block_ranges(Dev_mapper);
	free_bootcache();

	cleanup();
}

int main(int argc, char *argv[])
{
	set_cleanup(cleanup);
	Option.file_size = DEV_FILE_SIZE;
	Option.file = Dev_file;
	punyopt(argc, argv, NULL, NULL);

	normal();
	bad_map();
	meta_data_too_big();

	return 0;
}
