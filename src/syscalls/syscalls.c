#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "sys.h"

ssize_t
sys_write(struct TuxProc* proc, int fd, asuserptr_t bufp, size_t size)
{
    if (size == 0)
        return 0;
    assert(fd == 1);
    return write(1, (void*) bufp, size);
}

uintptr_t
sys_exit(struct TuxProc* proc, uint64_t val)
{
    pal_ctx_exit(proc->p_ctx, val);
    assert(!"unreachable");
}
