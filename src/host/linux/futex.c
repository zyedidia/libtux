#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "proc.h"

long
host_futexwait(struct TuxThread* p, uint32_t* uaddr, int op, uint32_t val, struct TimeSpec* timeout)
{
    struct timespec k_timeout = {0};
    long r = syscall(SYS_futex, uaddr, op, val, &k_timeout);
    timeout->sec = k_timeout.tv_sec;
    timeout->nsec = k_timeout.tv_nsec;
    return r;
}

long
host_futexwake(struct TuxThread* p, uint32_t* uaddr, int op, uint32_t val)
{
    return syscall(SYS_futex, uaddr, op, val);
}
