#include "realtime.h"

void stack_prefault(void) {
	unsigned char dummy[MAX_SAFE_STACK];
	memset(dummy, 0, MAX_SAFE_STACK);
}
