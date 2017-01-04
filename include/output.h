#ifndef CHARIORT_OUTPUT_H
#define CHARIORT_OUTPUT_H

#include <stdint.h>
#include "proftimer.h"

void outputJson(uint32_t tag_count, uint32_t intr_count, timeStamp tag_times[], double intr_times[], int size, char *flags, char *filename, int priority);
void outputDebug(uint32_t num_ints, uint32_t tag_count, uint32_t intr_count, timeStamp tag_times[], double intr_times[]);
void outputDebugTags(uint32_t tag_count, timeStamp tag_times[]);
void outputDebugInterrupts(uint32_t num_ints, uint32_t intr_count, double intr_times[]);

#endif //CHARIORT_OUTPUT_H
