#include <stdalign.h>
#include <errno.h>

// TODO: for readlinkat, not portable
#include <unistd.h>
#include <fcntl.h>

#include "syscalls/syscalls.h"

#include "file.h"
#include "fd.h"

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

struct IOVec {
    uintptr_t base;
    size_t len;
};

ssize_t
sys_writev(struct TuxProc* p, int fd, uintptr_t iovp, ssize_t iovcnt)
{
    uint8_t* iovb = procbufalign(p, iovp, iovcnt * sizeof(struct IOVec), alignof(struct IOVec));
    if (!iovb)
        return -TUX_EFAULT;
    struct IOVec* iov = (struct IOVec*) iovb;
    ssize_t total = 0;

    for (int i = 0; i < iovcnt; i++) {
        ssize_t n = sys_write(p, fd, iov[i].base, iov[i].len);
        if (n < 0) {
            return n;
        }
        total += n;
    }

    return total;
}

int
sys_openat(struct TuxProc* p, int dirfd, asuserptr_t pathp, int flags, int mode)
{
    if (dirfd != TUX_AT_FDCWD)
        return -TUX_EBADF;
    // TODO: copy from user to avoid TOCTOU
    const char* path = procpath(p, pathp);
    if (!path)
        return -TUX_EFAULT;
    struct FDFile* f = filenew(p->tux, p->cwd.name, path, flags, mode);
    VERBOSE(p->tux, "open %s\n", path);
    if (!f)
        return -TUX_ENOENT;
    int fd = fdalloc(&p->fdtable);
    if (fd < 0) {
        fdrelease(f, p);
        return -TUX_EMFILE;
    }
    fdassign(&p->fdtable, fd, f);
    return fd;
}

ssize_t
sys_readlinkat(struct TuxProc* p, int dirfd, asuserptr_t pathp, asuserptr_t bufp, size_t size)
{
    if (dirfd != TUX_AT_FDCWD)
        return -TUX_EINVAL;
    const char* path = procpath(p, pathp);
    uint8_t* buf = procbuf(p, bufp, size);
    if (!path || !buf)
        return -TUX_EFAULT;
    int r = readlinkat(AT_FDCWD, path, (char*) buf, size);
    if (r < 0)
        return -errno;
    return r;
}

ssize_t
sys_readlink(struct TuxProc* p, asuserptr_t pathp, asuserptr_t bufp, size_t size)
{
    return sys_readlinkat(p, TUX_AT_FDCWD, pathp, bufp, size);
}
