#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <linux/stddef.h>
#include "libchario.h"

struct chario_task task1;
struct chario_task task2;
struct chario_task task3;

struct chario_task *tasks[] = {&task1, &task2, &task3};

uint64_t *buffer;

int returnVal = 0;

int errors = 0;

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {

	printf("\n** libchario test started **\n\n");

	buffer = chario_init_device(true);
	if (buffer == 0) {
		return errno;
	}

	chario_init_task_with_blockcount(&task1, 1, 10);
	chario_init_task_with_blockcount(&task2, 2, 20);
	chario_init_task_with_blockcount(&task3, 3, 50);


//	for (int j = 0; j < 3; j++) {
//		for (size_t k = 0; k < tasks[j]->total_size/8; k++) {
//			buffer[k] = (uint64_t)tasks[j]->id << 4;
//		}
//		chario_flush_blocks_for_task(tasks[j]);
//	}


	uint64_t old_value;
	uint64_t new_value;

	printf("\n** Tasks initialised **\n\n");


	for (int i = 0; i < 10; i++) {
		printf("\n** Iteration %d **\n\n", i);

		for (int j = 0; j < 3; j++) {
			chario_schedule_task(tasks[j]);

			old_value = ((uint64_t)tasks[j]->id << 4 | i);
			new_value = old_value + 1;

			for (size_t k = 0; k < tasks[j]->total_size/8; k++) {
				if (buffer[k] == old_value) {
					//buffer[k]++;
					buffer[k] = new_value;
				}
				else {
					printf("Word %zu (0x%08x) is incorrect (0x%016llx)\n", k, (unsigned)&buffer[k], buffer[k]);
					returnVal = 1;
					errors++;
					if (errors > 10)
						goto end;
				}
			}

			chario_flush_blocks_for_task(tasks[j]);
		}
	}


end:
	chario_close_device();

	return returnVal;
}
