#ifndef LIBCHARIO_H
#define LIBCHARIO_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include "list.h"

//#define LIBCHARIO_DBG
#define LIBCHARIO_INFO

#define CHARIO_BLOCK_SIZE 4096
#define CHARIO_BUFFER_SIZE 0x10000000
#define CHARIO_DDR_START 0x20000000


struct chario_task {
    int id;
    struct list_head ranges;
    unsigned range_count;
    size_t total_count;
    size_t total_size;
};

struct chario_blocks_range {
    struct list_head list;
    off_t start;
    size_t count;
    size_t size;
    off_t buffer_offset;
};

struct chario_stack {
    struct list_head items;
    void *base;
    void *top;
    size_t total_size;
};

struct chario_stack_item {
    struct list_head list;
    struct chario_task *task;
};


uint64_t *chario_init_device(bool phys_addressing);
int chario_close_device(void);

int chario_init_task(struct chario_task *task, int id);

int chario_init_task_with_blockcount(struct chario_task *task, int id, size_t count);

int chario_add_range_to_task(struct chario_task *task, size_t count, off_t offset);

int chario_load_blocks_for_task(struct chario_task *task);

int chario_flush_blocks_for_task(struct chario_task *task);

uint64_t *buffer_for_range(struct chario_blocks_range *range);


#if defined(LIBCHARIO_DBG)
#define dbg_print(format, ...) printf("libchario: " format "\n", ##__VA_ARGS__)
#else
#define dbg_print(format, args...) {}
#endif

#if defined(LIBCHARIO_INFO) || defined(LIBCHARIO_DBG)
#define info_print(format, ...) printf("libchario: " format "\n", ##__VA_ARGS__)
#else
#define dbg_print(format, args...) {}
#endif

#endif //LIBCHARIO_H
