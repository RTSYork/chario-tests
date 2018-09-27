#include "simplefs.h"
#include "chario.h"



/**
 * Lookup a file by its id
 * @param id
 * @param file
 */
void file_lookup(uint32_t id, struct simplefs_file *file) {

}


/**
 * Create a file with start block and length
 * @param start Start block on disk
 * @param count Number of blocks in file
 * @return id of new file
 */
uint32_t file_create(off_t start, size_t length) {

	return 0;
}


/**
 * Destroy a file
 * @param file File to destroy
 */
void file_destroy(struct simplefs_file *file) {

}


/**
 * Load a file into memory
 * @param file File to load
 */
void file_load(struct simplefs_file *file) {

	file->flags.loaded = true;
}


/**
 * Unload a file from memory
 * @param file File to unload
 */
void file_unload(struct simplefs_file *file) {
	if (file->memory != 0 && file->flags.loaded) {

	}

	file->flags.loaded = false;
}


/**
 * Flush a file from memory to disk
 * @param file File to flush
 */
void file_flush(struct simplefs_file *file) {

}


/**
 * Fill a file with zeros on disk, and in memory if loaded
 * @param file File to zero
 */
void file_zero(struct simplefs_file *file) {

}


/**
 * Check that the file struct is valid
 * @param file File to validate
 * @return 0 if valid, error code otherwise
 */
int file_validate(struct simplefs_file *file) {

	return 0;
}


/**
 * Check if file is currently loaded
 * @param file File to check
 * @return true if file is loaded, false if not
 */
bool _file_is_loaded(struct simplefs_file *file) {

	return false;
}
