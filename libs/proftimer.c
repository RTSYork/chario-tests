#include <unistd.h>
#include "proftimer.h"
#include "proftimer-syscalls.h"

const char *tags[256];


void resetTimers(uint32_t *mem) {
	// Clear interrupt counter - set SCLR line then clock once
	mem[GPIO_DATA] = 0b000010;
	mem[GPIO_DATA] = 0b100110;
	mem[GPIO_DATA] = 0b000010;
	mem[GPIO_DATA] = 0b000000;

	// Clear profiling time source counter (SCLR must be high for one 125MHz clock)
	mem[GPIO_DATA] = 0b010000;
	mem[GPIO_DATA] = 0b000000;

	// Clear interrupt profiling timer
	mem[INTR_COMM_IN] = COMM_CLEAR;
	mem[INTR_COMM_IN] = 0;

	// Clear tagging profiling timer
	mem[PROF_TAG_IN] = 0;
	mem[PROF_COMM_IN] = COMM_CLEAR;
	mem[PROF_CTRL] = CTRL_START;
	while (!(mem[PROF_CTRL] & CTRL_DONE));
	mem[PROF_COMM_IN] = 0;

	// Wait for everything to settle
	usleep(10000);
}

uint32_t readTimers(uint32_t *mem, timeStamp times[]) {
	uint32_t count = mem[PROF_COUNT_OUT];
	for (uint32_t i = 0; i < count; i++) {
		mem[PROF_COMM_IN] = i + 1;
		mem[PROF_CTRL] = CTRL_START;
		while (!(mem[PROF_CTRL] & CTRL_DONE));
		uint64_t timeH = mem[PROF_TIMEH_OUT];
		uint64_t timeL = mem[PROF_TIMEL_OUT];
		uint64_t time = (timeH << 32) | timeL;
		times[i].time = time / 125000000.0;
		times[i].tag = (uint8_t)(mem[PROF_TAG_OUT] & 0xFF);
		times[i].data = mem[PROF_TAG_OUT] >> 8;
	}
	return count;
}

uint32_t readInterrupts(uint32_t *mem, double times[]) {
	while (!(mem[INTR_COUNT_OUT + 1] & CTRL_VLD));
	uint32_t count = mem[INTR_COUNT_OUT];
	for (uint32_t i = 0; i < count; i++) {
		mem[INTR_COMM_IN] = i + 1;
		while (!(mem[INTR_TIMEH_OUT + 1] & CTRL_VLD));
		while (!(mem[INTR_TIMEL_OUT + 1] & CTRL_VLD));
		uint64_t timeH = mem[INTR_TIMEH_OUT];
		uint64_t timeL = mem[INTR_TIMEL_OUT];
		uint64_t time = (timeH << 32) | timeL;
		times[i] = time / 125000000.0;
	}
	return count;
}

const char* label_for_tag(uint8_t tag) {
	if (!tags[0]) {
		tags[0x00] = "";
		tags[0x01] = "read-start";
		tags[0x02] = "read-end";
		tags[0x03] = "pcie_irq-start";
		tags[0x40] = "nvme_submit_iod-end";
		tags[0x41] = "nvme_submit_iod-wr";
		tags[0x42] = "nvme_submit_iod-rd";
		tags[0x05] = "nvme_irq-start";
		tags[0x06] = "userspace";
		tags[0x07] = "nvme_queue_rq-start";
		tags[0x08] = "nvme_irq-end";
		tags[0x09] = "nvme_irq-lock";
		tags[0x0b] = "__copy_to_user_memcpy-start";
		tags[0x0c] = "__copy_to_user_memcpy-end";
		tags[0x0d] = "pcie_irq-end";
		tags[0x0e] = "nvme_submit_iod-start";
		tags[0x0f] = "nvme_queue_rq-end";
		tags[0x10] = "dma_unmap_sg-done";
		tags[0x11] = "finished";
		tags[0x12] = "copy-start";
		tags[0x13] = "write-start";
		tags[0x14] = "write-end";
		tags[0x15] = "copy-end";
		tags[0x16] = "nvme_submit_cmd";
		tags[0x17] = "chario-cmd-submitted";
		tags[0x18] = "ioctl-start";
		tags[0x19] = "ioctl-end";
	}

	return tags[tag];
}

void enable_kernel_timers() {
	prof_timers_control(1);
}

void disable_kernel_timers() {
	prof_timers_control(0);
}
