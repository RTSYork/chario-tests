#ifndef CHARIORT_PROFTIMER_SYSCALLS_H
#define CHARIORT_PROFTIMER_SYSCALLS_H

#include <sys/syscall.h>

#define __NR_prof_timers_control 388
#define __NR_prof_read           389
#define __NR_prof_write          390

#if __APPLE__

#define prof_timers_control(enable) 0
#define prof_read(fd, buf, count) 0
#define prof_write(fd, buf, count) 0

#else

#define prof_timers_control(enable) syscall(__NR_prof_timers_control, enable)
#define prof_read(fd, buf, count) syscall(__NR_prof_read, fd, buf, count)
#define prof_write(fd, buf, count) syscall(__NR_prof_write, fd, buf, count)

#endif

#endif //CHARIORT_PROFTIMER_SYSCALLS_H
