#include <stdio.h>
#include "output.h"


void outputJson(uint32_t tag_count, uint32_t intr_count, timeStamp tag_times[], double intr_times[], int size, char *flags, char *filename, int priority) {
	printf("  {\n    \"tags\": [\n");
	if (tag_count > 0) {
		for (uint32_t i = 0; i < tag_count - 1; i++) {
			printf("      { \"data\": %d, \"tag\": %d, \"label\": \"%s\", \"time\": %.9f },\n", tag_times[i].data,
			       tag_times[i].tag, label_for_tag(tag_times[i].tag), tag_times[i].time);
		}
		for (uint32_t i = tag_count - 1; i < tag_count; i++) {
			printf("      { \"data\": %d, \"tag\": %d, \"label\": \"%s\", \"time\": %.9f }\n", tag_times[i].data,
			       tag_times[i].tag, label_for_tag(tag_times[i].tag), tag_times[i].time);
		}
	}
	printf("    ],\n");
	printf("    \"interrupts\": [\n");
	if (intr_count > 0) {
		for (uint32_t i = 0; i < intr_count - 1; i++) {
			printf("      %.9f,\n", intr_times[i]);
		}
		for (uint32_t i = intr_count - 1; i < intr_count; i++) {
			printf("      %.9f\n", intr_times[i]);
		}
	}
	printf("    ],\n");
	printf("    \"bytes\": %d,\n", size);
	printf("    \"flags\": \"%s\",\n", flags);
	printf("    \"file\": \"%s\",\n", filename);
	printf("    \"priority\": %d\n", priority);
	printf("  }\n");
}


void outputDebug(uint32_t num_ints, uint32_t tag_count, uint32_t intr_count, timeStamp tag_times[], double intr_times[]) {
	// Raw number of counted interrupts
	printf("%d raw interrupts counted\n", num_ints);

	// Print out tag times
	printf("%d events tagged\n", tag_count);
	for (uint32_t i = 0; i < tag_count; i++) {
		printf("%3d: 0x%06x 0x%02x %s = %.9f\n", i + 1, tag_times[i].data, tag_times[i].tag,
		       label_for_tag(tag_times[i].tag), tag_times[i].time);
	}

	// Print out interrupt times
	printf("%d interrupts timed\n", intr_count);
	for (uint32_t i = 0; i < intr_count; i++) {
		printf("%3d: %.9f\n", i + 1, intr_times[i]);
	}
}

void outputDebugTags(uint32_t tag_count, timeStamp tag_times[]) {
	// Print out tag times
	printf("%d events tagged\n", tag_count);
	for (uint32_t i = 0; i < tag_count; i++) {
		printf("%3d: 0x%06x 0x%02x %s = %.9f\n", i + 1, tag_times[i].data, tag_times[i].tag,
		       label_for_tag(tag_times[i].tag), tag_times[i].time);
	}
}

void outputDebugInterrupts(uint32_t num_ints, uint32_t intr_count, double intr_times[]) {
	// Raw number of counted interrupts
	printf("%d raw interrupts counted\n", num_ints);

	// Print out interrupt times
	printf("%d interrupts timed\n", intr_count);
	for (uint32_t i = 0; i < intr_count; i++) {
		printf("%3d: %.9f\n", i + 1, intr_times[i]);
	}
}
