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

ssize_t
sys_read(struct TuxProc* p, int fd, asuserptr_t bufp, size_t size)
{
    if (size == 0)
        return 0;
    struct FDFile* f = fdget(&p->fdtable, fd);
    if (!f)
        return -TUX_EBADF;
    if (!f->read)
        return -TUX_EPERM;
    uint8_t* buf = procbuf(p, bufp, size);
    if (!buf)
        return -TUX_EFAULT;
    return f->read(f->dev, p, buf, size);
}

struct IOVec {
    uintptr_t base;
    size_t len;
};

ssize_t
sys_writev(struct TuxProc* p, int fd, asuserptr_t iovp, size_t iovcnt)
{
    uint8_t* iovb = procbufalign(p, iovp, iovcnt * sizeof(struct IOVec), alignof(struct IOVec));
    if (!iovb)
        return -TUX_EFAULT;
    struct IOVec* iov = (struct IOVec*) iovb;
    ssize_t total = 0;

    for (size_t i = 0; i < iovcnt; i++) {
        ssize_t n = sys_write(p, fd, iov[i].base, iov[i].len);
        if (n < 0) {
            return n;
        }
        total += n;
    }

    return total;
}

ssize_t
sys_readv(struct TuxProc* p, int fd, asuserptr_t iovp, size_t iovcnt)
{
    uint8_t* iovb = procbufalign(p, iovp, iovcnt * sizeof(struct IOVec), alignof(struct IOVec));
    if (!iovb)
        return -TUX_EFAULT;
    struct IOVec* iov = (struct IOVec*) iovb;
    ssize_t total = 0;

    for (size_t i = 0; i < iovcnt; i++) {
        ssize_t n = sys_read(p, fd, iov[i].base, iov[i].len);
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
    if (!f) {
        VERBOSE(p->tux, "sys_open(\"%s\") = %d", path, -TUX_ENOENT);
        return -TUX_ENOENT;
    }
    int fd = fdalloc(&p->fdtable);
    if (fd < 0) {
        fdrelease(f, p);
        VERBOSE(p->tux, "sys_open(\"%s\") = %d", path, -TUX_EMFILE);
        return -TUX_EMFILE;
    }
    VERBOSE(p->tux, "sys_open(\"%s\") = %d", path, fd);
    fdassign(&p->fdtable, fd, f);
    return fd;
}

int
sys_close(struct TuxProc* p, int fd)
{
    bool ok = fdremove(&p->fdtable, fd, p);
    if (!ok)
        return -TUX_EBADF;
    return 0;
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

ssize_t
sys_pread64(struct TuxProc* p, int fd, asuserptr_t bufp, size_t size, ssize_t offset)
{
    struct FDFile* f = fdget(&p->fdtable, fd);
    if (!f)
        return -TUX_EBADF;
    if (!f->read || !f->lseek)
        return -TUX_EPERM;
    uint8_t* buf = procbuf(p, bufp, size);
    if (!buf)
        return -TUX_EFAULT;
    ssize_t orig = f->lseek(f->dev, p, 0, TUX_SEEK_CUR);
    f->lseek(f->dev, p, offset, TUX_SEEK_SET);
    ssize_t n = f->read(f->dev, p, buf, size);
    f->lseek(f->dev, p, orig, TUX_SEEK_SET);
    return n;
}

int
sys_newfstatat(struct TuxProc* p, int dirfd, asuserptr_t pathp, asuserptr_t statbufp, int flags)
{
    uint8_t* statb = procbufalign(p, statbufp, sizeof(struct stat), alignof(struct stat));
    if (!statb)
        return -TUX_EFAULT;
    struct Stat* stat = (struct Stat*) statb;
    if ((flags & TUX_AT_EMPTY_PATH) == 0) {
        const char* path = procpath(p, pathp);
        if (!path)
            return -TUX_EFAULT;
        if (dirfd != AT_FDCWD)
            return -TUX_EBADF;
        return syserr(filefstatat(p->cwd.name, path, stat, flags));
    }
    struct FDFile* f = fdget(&p->fdtable, dirfd);
    if (!f)
        return -TUX_EBADF;
    if (!f->stat_)
        return -TUX_EACCES;
    return f->stat_(f->dev, p, stat);
}

ssize_t
sys_getdents64(struct TuxProc* p, int fd, asuserptr_t dirp, size_t count)
{
    struct FDFile* f = fdget(&p->fdtable, fd);
    if (!f)
        return -TUX_EBADF;
    if (!f->getdents)
        return -TUX_EPERM;
    uint8_t* buf = procbuf(p, dirp, count);
    if (!buf)
        return -TUX_EFAULT;
    return f->getdents(f->dev, p, buf, count);
}

off_t
sys_lseek(struct TuxProc* p, int fd, off_t offset, int whence)
{
    struct FDFile* f = fdget(&p->fdtable, fd);
    if (!f)
        return -TUX_EBADF;
    if (!f->lseek)
        return -TUX_EPERM;
    return f->lseek(f->dev, p, offset, whence);
}
