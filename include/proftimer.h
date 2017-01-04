#ifndef CHARIORT_PROFTIMER_H
#define CHARIORT_PROFTIMER_H

#include <stdint.h>

#define O_PROFILE 01000000000

#define PROFTIMER_BASE 0x50000000
#define PROFTIMER_SIZE 0x3000

#define GPIO_DATA  0x0800
#define GPIO2_DATA 0x0802

#define PROF_CTRL      0x0000
#define PROF_COMM_IN   0x0004
#define PROF_TAG_IN    0x0006
#define PROF_TAG_OUT   0x0008
#define PROF_TIMEH_OUT 0x000A
#define PROF_TIMEL_OUT 0x000C
#define PROF_COUNT_OUT 0x000E

#define INTR_COMM_IN   0x0404
#define INTR_TIMEH_OUT 0x0406
#define INTR_TIMEL_OUT 0x0408
#define INTR_COUNT_OUT 0x040A

#define COMM_CLEAR 0xFFFFFFFF
#define CTRL_START 0b00000001
#define CTRL_DONE  0b00000010
#define CTRL_VLD   0b00000001


typedef struct {
	double time;
	uint8_t tag;
	uint32_t data;
} timeStamp;


void resetTimers(uint32_t *mem);

uint32_t readTimers(uint32_t *mem, timeStamp times[]);
uint32_t readInterrupts(uint32_t *mem, double times[]);

const char* label_for_tag(uint8_t tag);

void enable_kernel_timers(void);
void disable_kernel_timers(void);


static inline void tag(uint32_t *mem, uint32_t tag) {
	mem[PROF_TAG_IN] = tag;
	mem[PROF_CTRL] = CTRL_START;
}

static inline void stop_counters(uint32_t *mem) {
	mem[GPIO_DATA] = 0b000000;
}

static inline void start_counters(uint32_t *mem) {
	// Set interrupt counter and timer CE (bit 0, bit 3) and stop holding down interrupt line (bit 5)
	mem[GPIO_DATA] = 0b101001;
}

static inline void start_tag_timer(uint32_t *mem) {
	// Set interrupt counter and timer CE (bit 0, bit 3) and stop holding down interrupt line (bit 5)
	mem[GPIO_DATA] = 0b001000;
}

static inline uint32_t get_raw_intr_count(uint32_t *mem)
{
	return mem[GPIO2_DATA];
}

#endif // CHARIORT_PROFTIMER_H