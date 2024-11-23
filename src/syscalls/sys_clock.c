#include <stdalign.h>
#include <time.h>
#include <errno.h>

#include "syscalls/syscalls.h"

enum {
    TUX_CLOCK_REALTIME  = 0,
    TUX_CLOCK_MONOTONIC = 1,
};

typedef int64_t tux_time_t;

struct TuxTimeSpec {
    tux_time_t tv_sec;
    long tv_nsec;
};

int
sys_clock_gettime(struct TuxProc* p, tux_clockid_t clockid, uintptr_t tp) {
    if (clockid != TUX_CLOCK_REALTIME && clockid != TUX_CLOCK_MONOTONIC)
        return -TUX_EINVAL;
    uint8_t* tb = procbufalign(p, tp, sizeof(struct TuxTimeSpec), alignof(struct TuxTimeSpec));
    if (!tb)
        return -TUX_EFAULT;
    struct TuxTimeSpec* tux_t = (struct TuxTimeSpec*) tb;
    struct timespec t;
    int err = clock_gettime(clockid, &t);
    if (err < 0)
        return -errno;
    tux_t->tv_sec = t.tv_sec;
    tux_t->tv_nsec = t.tv_nsec;
    return 0;
}
