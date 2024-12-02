#include <assert.h>

#include "syscalls/syscalls.h"

int
sys_rt_sigaction(struct TuxProc* p, int sig, int64_t act, int64_t old, uint64_t sigsetsize)
{
    assert(!"unimplemented: rt_sigaction");
}

int
sys_rt_sigprocmask(struct TuxProc* p, int how, int64_t setaddr, int64_t oldsetaddr, uint64_t sigsetsize)
{
    assert(!"unimplemented: rt_sigprocmask");
}

int
sys_rt_sigreturn(struct TuxProc* p)
{
    assert(!"unimplemented: rt_sigreturn");
}
