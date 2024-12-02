#include <assert.h>

#include "host.h"
#include "syscalls/syscalls.h"

static long
futexwait(struct TuxProc* p, asuserptr_t uaddrp, int op, uint32_t val, asuserptr_t timeoutp)
{
    assert(!"unimplemented: futexwait");
}

static long
futexwake(struct TuxProc* p, asuserptr_t uaddrp, uint32_t val)
{
    assert(!"unimplemented: futexwake");
}

long
sys_futex(struct TuxProc* p, asuserptr_t uaddrp, int op, uint32_t val,
        uint64_t timeoutp, asuserptr_t uaddr2p, uint32_t val3)
{
    if (uaddrp & 3)
        return -TUX_EFAULT;
    op &= ~TUX_FUTEX_PRIVATE_FLAG;
    switch (op) {
    case TUX_FUTEX_WAIT:
        return futexwait(p, uaddrp, op, val, timeoutp);
    case TUX_FUTEX_WAKE:
        return futexwake(p, uaddrp, val);
    default:
        return -TUX_EINVAL;
    }
}

int
sys_clone(struct TuxProc* p, uint64_t flags, uint64_t stack, uint64_t ptid, uint64_t ctid, uint64_t tls, uint64_t func)
{
    assert(!"unimplemented: sys_clone");
}

int
sys_sched_getaffinity(struct TuxProc* p, int32_t pid, uint64_t cpusetsize, int64_t maskaddr)
{
    assert(!"unimplemented: sched_getaffinity");
}

int
sys_sched_setaffinity(struct TuxProc* p, int32_t pid, uint64_t cpusetsize, int64_t maskaddr)
{
    assert(!"unimplemented: sched_setaffinity");
}

int
sys_sched_yield(struct TuxProc* p)
{
    return host_sched_yield();
}
