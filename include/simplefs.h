#ifndef SIMPLEFS_H
#define SIMPLEFS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

struct simplefs_file_flags {
	bool loaded : 1;
};

/**
 * Struct to hold file information
 */
struct simplefs_file {
	uint32_t id;                      /**< file id number */
	struct simplefs_file_flags flags; /**< file flags */
	int position;                     /**< position in files array */
	off_t start;                      /**< start block number */
	size_t length;                    /**< length in blocks */
	void *memory;                     /**< location if loaded in memory */
};

/**
 * Struct to hold on-disk file information
 */
struct simplefs_file {
	uint32_t id;     /**< file id number */
	uint32_t flags;  /**< file flags */
	uint32_t start;  /**< start block number */
	uint32_t length; /**< length in blocks */
};

void file_lookup(uint32_t id, struct simplefs_file *file);
uint32_t file_create(off_t start, size_t length);
void file_destroy(struct simplefs_file *file);
void file_load(struct simplefs_file *file);
void file_unload(struct simplefs_file *file);
void file_flush(struct simplefs_file *file);
void file_zero(struct simplefs_file *file);
int file_validate(struct simplefs_file *file);

bool _file_is_loaded(struct simplefs_file *file);

#endif //SIMPLEFS_H
