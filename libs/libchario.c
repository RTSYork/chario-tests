#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/types.h>
#include "libchario.h"
#include "chario.h"

int fd = -1;
int mfd = -1;
uint64_t *buffer;
bool use_phys;
struct chario_stack stack;

off_t device_offset;


uint64_t *chario_init_device(bool phys_addressing) {
	dbg_print("chario_init_device(%d)", phys);
	fd = open("/dev/chardisk0", O_RDWR);
	dbg_print("    fd = %d", fd);
	if (fd < 0) {
		perror("Failed to open the device");
		return 0;
	}

	use_phys = phys_addressing;

	if (use_phys) {
		mfd = open("/dev/mem", O_RDWR | O_SYNC);
		dbg_print("    mfd = %d", mfd);
		buffer = mmap(NULL, CHARIO_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mfd, CHARIO_DDR_START);
//	        memset(buffer, 0, DDR_SIZE);
	}
	else {
		buffer = aligned_alloc(CHARIO_BLOCK_SIZE, CHARIO_BUFFER_SIZE);
//	        memset(buffer, 0, DDR_SIZE);
	}
	dbg_print("    buffer = 0x%08x", (unsigned)buffer);

	stack.base = (off_t)buffer;
	stack.top = stack.base;
	stack.total_size = CHARIO_BUFFER_SIZE;
	INIT_LIST_HEAD(&stack.items);

	return buffer;
}


int chario_close_device(void) {
	dbg_print("chario_close_device");
	if (use_phys) {
		munmap(buffer, CHARIO_BUFFER_SIZE);
	}
	else {
		free(buffer);
	}
	buffer = 0;
	close(mfd);
	mfd = -1;
	close(fd);
	fd = -1;
	return 0;
}


int chario_init_task(struct chario_task *task, int id) {
	dbg_print("chario_init_task(0x%08x, %d)", (unsigned)task, id);
	task->id = id;
	INIT_LIST_HEAD(&task->ranges);
	return 0;
}

int chario_init_task_with_blockcount(struct chario_task *task, int id, size_t count) {
	int result;

	dbg_print("chario_init_task_with_blockcount(0x%08x, %d, %zu)", (unsigned)task, id, size);
	task->id = id;
	INIT_LIST_HEAD(&task->ranges);

	result = chario_add_range_to_task(task, count, device_offset);

	if (result == 0) {
		device_offset += count;
	}

	return result;
}


int chario_add_range_to_task(struct chario_task *task, size_t count, off_t offset) {
	struct chario_blocks_range *range;
	struct chario_blocks_range *prev_item;

	dbg_print("chario_add_range_to_task(%d, %zu, %jd)", task->id, count, (intmax_t)offset);

	range = malloc(sizeof(struct chario_blocks_range));

	range->start = offset;
	range->count = count;
	range->size = count * CHARIO_BLOCK_SIZE;

	if (list_empty(&task->ranges)) {
		range->buffer_offset = 0;
	}
	else {
		prev_item = list_last_entry(&task->ranges, struct chario_blocks_range, list);
		range->buffer_offset = (off_t)(prev_item->buffer_offset + (prev_item->count * CHARIO_BLOCK_SIZE));
	}

	list_add_tail(&range->list, &task->ranges);
	task->range_count++;
	task->total_count += range->count;
	task->total_size += range->size;

	dbg_print("    range->buffer_offset = %jd", (intmax_t)range->buffer_offset);

	return 0;
}


#ifdef __clang__
#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"
#endif
int chario_load_blocks_for_task(struct chario_task *task) {
	struct chario_blocks_range *range;
	int __attribute__((unused)) i = 0 ;
	ssize_t count;

	info_print("chario_load_blocks_for_task(%d)", task->id);

	if (fd == -1 || buffer == 0) {
		dbg_print("    Device not initialised");
		return 1;
	}

	list_for_each_entry(range, &task->ranges, list) {
		dbg_print("    item %d", i++);
		dbg_print("    range = 0x%08x", (unsigned)range);
		dbg_print("        range->start = %jd", (intmax_t)range->start);
		dbg_print("        range->count = %zu", range->count);
		dbg_print("        range->size = %zu", range->size);
		dbg_print("        range->buffer_offset = %jd", (intmax_t)range->buffer_offset);

		if (use_phys) {
			struct chario_phys_io io = {
				.address = (__u64) (CHARIO_DDR_START + range->buffer_offset),
				.length = (size_t) (range->size)
			};
			lseek(fd, range->start * CHARIO_BLOCK_SIZE, SEEK_SET);
			count = ioctl(fd, CHARIO_IOCTL_READ_PHYS, &io);
			info_print("        read %zd of %zu bytes to 0x%08x (0x%08x)", count,
			           range->size, (unsigned) (CHARIO_DDR_START + range->buffer_offset),
			           (unsigned) (buffer + range->buffer_offset));
		}
		else {
			lseek(fd, range->start * CHARIO_BLOCK_SIZE, SEEK_SET);
			count = read(fd, buffer + (range->buffer_offset / 8), range->size);
			info_print("        read %zd of %zu bytes to 0x%08x", count, range->size,
			           (unsigned) (buffer + (range->buffer_offset / 8)));
		}
	}

	return 0;
}


int chario_flush_blocks_for_task(struct chario_task *task) {
	struct chario_blocks_range *range;
	int __attribute__((unused)) i = 0;
	ssize_t count;

	info_print("chario_flush_blocks_for_task(%d)", task->id);

	if (fd == -1 || buffer == 0) {
		dbg_print("    Device not initialised");
		return 1;
	}

	list_for_each_entry(range, &task->ranges, list) {
		dbg_print("    item %d", i++);
		dbg_print("    range = 0x%08x", (unsigned)range);
		dbg_print("        range->start = %jd", (intmax_t)range->start);
		dbg_print("        range->count = %zu", range->count);
		dbg_print("        range->size = %zu", range->size);
		dbg_print("        range->buffer_offset = %jd", (intmax_t)range->buffer_offset);

		if (use_phys) {
			struct chario_phys_io io = {
				.address = (__u64) (CHARIO_DDR_START + range->buffer_offset),
				.length = (size_t) (range->size)
			};
			lseek(fd, range->start * CHARIO_BLOCK_SIZE, SEEK_SET);
			count = ioctl(fd, CHARIO_IOCTL_WRITE_PHYS, &io);
			info_print("        wrote %zd of %zu bytes from 0x%08x (0x%08x)", count,
			           range->size, (unsigned) (CHARIO_DDR_START + range->buffer_offset),
			           (unsigned) (buffer + range->buffer_offset));
		}
		else {
			lseek(fd, range->start * CHARIO_BLOCK_SIZE, SEEK_SET);
			count = write(fd, buffer + (range->buffer_offset / 8), range->size);
			info_print("        wrote %zd of %zu bytes from 0x%08x", count,
			           range->size, (unsigned) (buffer + (range->buffer_offset / 8)));
		}
	}

	return 0;
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

uint64_t *buffer_for_range(struct chario_blocks_range *range) {
	info_print("buffer_for_range(%jd)",(intmax_t)range->start);

	if (buffer == 0)
		return 0;

	return buffer + (range->buffer_offset/8);
}
