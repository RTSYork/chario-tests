#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <linux/stddef.h>
#include "simplefs.h"


int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {

	printf("\n** simplefs test 1 started **\n\n");

	struct simplefs_file file1;
	uint32_t id1;
	int valid;

	id1 = file_create(0, 1);

	file_lookup(id1, &file1);

	valid = file_validate(&file1);

	file_load(&file1);

	file_zero(&file1);

	file_flush(&file1);

	file_unload(&file1);

	file_destroy(&file1);


	printf("\n\n** simplefs test 1 done **\n\n");
	return 0;
}
