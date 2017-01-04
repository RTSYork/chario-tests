#ifndef CHARIORT_REALTIME_H
#define CHARIORT_REALTIME_H

#include <time.h>
#include <string.h>
#include <sched.h>
#include <time.h>

#define NSEC_PER_SEC 1000000000 /* The number of nsecs per sec. */

#define MAX_SAFE_STACK (8*1024) /* The maximum stack size which is
								   guaranteed safe to access without
								   faulting */

/* the struct timespec consists of nanoseconds
 * and seconds. if the nanoseconds are getting
 * bigger than 1000000000 (= 1 second) the
 * variable containing seconds has to be
 * incremented and the nanoseconds decremented
 * by 1000000000.
 */
static inline void tsnorm(struct timespec *ts)
{
	while (ts->tv_nsec >= NSEC_PER_SEC) {
		ts->tv_nsec -= NSEC_PER_SEC;
		ts->tv_sec++;
	}
}

void stack_prefault(void);


// Some stubs so things build on OS X
#if __APPLE__

#define TIMER_ABSTIME 0

typedef int pid_t;

static inline int sched_setscheduler(pid_t pid __attribute__ ((unused)), int policy __attribute__ ((unused)), const struct sched_param *param __attribute__ ((unused))) {
	return 0;
}

static inline int clock_nanosleep(clockid_t clock_id __attribute__ ((unused)), int flags __attribute__ ((unused)), const struct timespec *request __attribute__ ((unused)), struct timespec *remain __attribute__ ((unused))) {
	return 0;
}

#endif


#endif //CHARIORT_REALTIME_H
