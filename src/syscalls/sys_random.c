#include <errno.h>
#include <sys/random.h>

#include "syscalls/syscalls.h"

ssize_t
sys_getrandom(struct TuxProc* p, asuserptr_t bufp, size_t buflen, unsigned int flags)
{
    uint8_t* buf = procbuf(p, bufp, buflen);
    if (!buf)
        return -TUX_EINVAL;
    // TODO: not portable
    int err = getrandom(buf, buflen, flags);
    if (err < 0)
        return -errno;
    return 0;
}
