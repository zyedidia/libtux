#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "fd.h"
#include "sys.h"

static uint8_t*
procbuf(struct TuxProc* p, asuserptr_t bufp, size_t size)
{
    void* buf = pal_as_user2p(p->p_as, bufp);
    return (uint8_t*) buf;
}

static void*
procbufalign(struct TuxProc* p, asuserptr_t bufp, size_t size, size_t align)
{
    void* buf = pal_as_user2p(p->p_as, bufp);
    if (!buf)
        return NULL;
    if ((uintptr_t) buf % align != 0)
        return NULL;
    return (void*) buf;
}

ssize_t
sys_write(struct TuxProc* p, int fd, asuserptr_t bufp, size_t size)
{
    if (size == 0)
        return 0;
    struct FDFile* f = fdget(&p->fdtable, fd);
    if (!f)
        return -TUX_EBADF;
    if (!f->write)
        return -TUX_EPERM;
    uint8_t* buf = procbuf(p, bufp, size);
    if (!buf)
        return -TUX_EFAULT;
    return f->write(f->dev, p, buf, size);
}

uintptr_t
sys_exit(struct TuxProc* p, uint64_t val)
{
    pal_ctx_exit(p->p_ctx, val);
    assert(!"unreachable");
}
