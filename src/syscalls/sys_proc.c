#include <assert.h>

#include "syscalls/syscalls.h"

void
sys_exit(struct TuxProc* p, int code)
{
    pal_ctx_exit(p->p_ctx, code);
    assert(!"unreachable");
}

void
sys_exit_group(struct TuxProc* p, int code)
{
    pal_ctx_exit(p->p_ctx, code);
    assert(!"unreachable");
}
