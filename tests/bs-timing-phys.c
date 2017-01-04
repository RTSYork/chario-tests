#define _FILE_OFFSET_BITS 64
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "proftimer.h"
#include "chario.h"
#include "output.h"

#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))

#define RAND_SEED 1
#define BLOCK 4096
#define DDR_START 0x20000000
#define DDR_SIZE 0x20000000 // 512MiB
#define MAX_BUFF_OFFSET ((DDR_SIZE-(2*1024*1024))/BLOCK) // 510MiB / 4KiB blocks
#define DEV_SECTS 97677846 // 97677846 sectors * 4096 bytes = 400088457216 bytes (~400GB SSD)
#define MAX_DEV_OFFSET (DEV_SECTS-512) // 512 sectors (2MiB) from end of device

void cleanup(void);

uint8_t *buff;
int fd;
int mfd;
uint32_t *proftimers_mem;
int dev_offset;
off_t f_offset;
int buff_offset;
off_t lseek_result;
ssize_t read_result;
size_t read_size;
size_t read_sizes[] = {BLOCK, BLOCK*2, BLOCK*4, BLOCK*8, BLOCK*16, BLOCK*32, BLOCK*64, BLOCK*128, BLOCK*256, BLOCK*512};
int num_sizes = NELEMS(read_sizes);
timeStamp tag_times[65536];
struct chario_phys_io io;

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {

	fprintf(stderr, "-- CharIORT Block Size Test (with physical addressing) --\n");

	mfd = open("/dev/mem", O_RDWR | O_SYNC);
	if (mfd == -1) {
		perror("Can't open /dev/mem");
		cleanup();
		return 1;
	}

	proftimers_mem = mmap(NULL, PROFTIMER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mfd, PROFTIMER_BASE);
	if (proftimers_mem == MAP_FAILED) {
		perror("Can't map timers memory");
		cleanup();
		return 1;
	}

	buff = mmap(NULL, DDR_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mfd, DDR_START);
	if (buff == MAP_FAILED) {
		perror("Can't map buffer memory");
		cleanup();
		return 1;
	}

	fd = open("/dev/chardisk0", O_RDWR);
	if (fd < 0) {
		perror("Failed to open the device");
		cleanup();
		return 1;
	}

	srand(RAND_SEED);

	resetTimers(proftimers_mem);

	fprintf(stderr, "Running...\n");

	start_tag_timer(proftimers_mem);

	for (int i = 0; i < 3000; i++) {
		for (int j = 0; j < num_sizes; j++) {
			read_size = read_sizes[j];
			dev_offset = rand() % MAX_DEV_OFFSET;
			f_offset = (off_t) dev_offset * BLOCK;
			buff_offset = (rand() % MAX_BUFF_OFFSET) * BLOCK;
			lseek_result = lseek(fd, f_offset, SEEK_SET);
			if (lseek_result != f_offset) {
				perror("Error seeking device file");
				break;
			}

			io.address = (__u64)(DDR_START + buff_offset);
			io.length = read_size;

			tag(proftimers_mem, 0x01); // read-start

			read_result = ioctl(fd, CHARIO_IOCTL_READ_PHYS, &io);

			tag(proftimers_mem, 0x02); // read-end

			if (read_result != (ssize_t)read_size) {
				perror("Error reading from device");
				fprintf(stderr, "result: %zd\n", read_result);
				break;
			}
		}
	}

	stop_counters(proftimers_mem);

	// Read tag counts and timestamps
	uint32_t tag_count = readTimers(proftimers_mem, tag_times);
	fprintf(stderr, "Outputting data from %u tags...\n", tag_count);

//	outputDebugTags(tag_count, tag_times);

	double time1, time2, diff, blocks, block_time;

	printf("size,read-start,read-end,time,block-time\n");
	for (uint32_t i = 0; i < tag_count; i+=2) {
		read_size = read_sizes[(i / 2) % num_sizes];
		blocks = read_size / BLOCK;

		if (tag_times[i].tag == 0x01 && tag_times[i+1].tag == 0x02) {
			time1 = tag_times[i].time;
			time2 = tag_times[i+1].time;
			diff = time2 - time1;
			block_time = diff / blocks;

			printf("%zd,%.9f,%.9f,%.9f,%.9f\n", read_size, time1, time2, diff, block_time);
		}
		else {
			printf("*** ERROR IN TAG %u ***\n", i);
			fprintf(stderr, "*** ERROR IN TAG %u ***\n", i);
		}
	}


	cleanup();

	fprintf(stderr, "Done.\n");

	return 0;
}

void cleanup() {
	fprintf(stderr, "Cleaning up...\n");
	disable_kernel_timers();
	munmap(proftimers_mem, PROFTIMER_SIZE);
	munmap(buff, DDR_SIZE);
	close(mfd);
	close(fd);
}
