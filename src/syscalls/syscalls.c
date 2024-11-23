#include <assert.h>
#include <stdalign.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <sys/random.h>

#include "file.h"
#include "fd.h"
#include "sys.h"
#include "syscalls/syscalls.h"

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

uintptr_t
sys_exit(struct TuxProc* p, uint64_t val)
{
    pal_ctx_exit(p->p_ctx, val);
    assert(!"unreachable");
}

uintptr_t
sys_brk(struct TuxProc* p, asuserptr_t addr)
{
    asptr_t brkp = p->brkbase + p->brksize;
    if (addr != 0)
        brkp = procaddr(p, addr);
    if (brkp < p->brkbase)
        brkp = p->brkbase;
    if (brkp >= p->brkbase + TUX_BRKMAXSIZE)
        brkp = p->brkbase + TUX_BRKMAXSIZE;

    size_t newsize = brkp - p->brkbase;
    assert(newsize < TUX_BRKMAXSIZE);

    if (newsize == p->brksize)
        return procuseraddr(p, brkp);

    const int mapflags = PAL_MAP_PRIVATE | PAL_MAP_ANONYMOUS;
    const int mapprot = PAL_PROT_READ | PAL_PROT_WRITE;

    if (brkp >= p->brkbase + p->brksize) {
        asptr_t map;
        if (p->brksize == 0) {
            map = pal_as_mapat(p->p_as, p->brkbase, newsize, mapprot, mapflags, -1, 0);
        } else {
            // First unmap where we want to expand to, and then mm
            map = pal_as_mapat(p->p_as, p->brkbase + p->brksize, newsize - p->brksize, mapprot, mapflags, -1, 0);
        }
        if (map == (asptr_t) -1)
            return -1;
    }
    p->brksize = newsize;
    return procuseraddr(p, p->brkbase + p->brksize);
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
    printf("open %s\n", path);
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

enum {
    UTSNAME_LENGTH = 65,
};

struct UTSName {
    char sysname[UTSNAME_LENGTH];
    char nodename[UTSNAME_LENGTH];
    char release[UTSNAME_LENGTH];
    char version[UTSNAME_LENGTH];
    char machine[UTSNAME_LENGTH];
};

int
sys_uname(struct TuxProc* p, asuserptr_t bufp)
{
    uint8_t* utsb = procbufalign(p, bufp, sizeof(struct UTSName), alignof(struct UTSName));
    if (!utsb)
        return -TUX_EFAULT;
    struct UTSName* uts = (struct UTSName*) utsb;
    strncpy(uts->sysname, "Linux", UTSNAME_LENGTH);
    strncpy(uts->release, "6.0.0-libtux", UTSNAME_LENGTH);
    uts->nodename[0] = 0;
    uts->version[0] = 0;
    uts->machine[0] = 0;
    return 0;
}

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
