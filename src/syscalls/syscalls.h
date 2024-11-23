#pragma once

#include <string.h>

#include "tux_pal.h"
#include "proc.h"

enum {
    TUX_EPERM  = 1,
    TUX_ENOENT = 2,
    TUX_EBADF  = 9,
    TUX_EFAULT = 14,
    TUX_EINVAL = 22,
    TUX_EMFILE = 24,
    TUX_ENOSYS = 38,
};

static inline asptr_t
procaddr(struct TuxProc* p, asuserptr_t addr)
{
    return pal_as_user2ap(p->p_as, addr);
}

static inline asuserptr_t
procuseraddr(struct TuxProc* p, asptr_t addr)
{
    return pal_as_ap2user(p->p_as, addr);
}

static inline uint8_t*
procbuf(struct TuxProc* p, asuserptr_t bufp, size_t size)
{
    void* buf = pal_as_user2p(p->p_as, bufp);
    return (uint8_t*) buf;
}

static inline void*
procbufalign(struct TuxProc* p, asuserptr_t bufp, size_t size, size_t align)
{
    void* buf = pal_as_user2p(p->p_as, bufp);
    if (!buf)
        return NULL;
    if ((uintptr_t) buf % align != 0)
        return NULL;
    return (void*) buf;
}

static inline const char*
procpath(struct TuxProc* p, asuserptr_t pathp)
{
    void* path = pal_as_user2p(p->p_as, pathp);
    if (!path)
        return NULL;
    const char* str = (const char*) path;
    size_t len = strnlen(str, TUX_PATH_MAX);
    if ((uintptr_t) str + len >= p->p_info.maxaddr)
        return NULL;
    if (str[len] != 0)
        return NULL;
    return str;
}

ssize_t sys_write(struct TuxProc* p, int fd, asuserptr_t bufp, size_t size);

uintptr_t sys_exit(struct TuxProc* p, uint64_t val);

uintptr_t sys_brk(struct TuxProc* p, asuserptr_t addr);

int sys_openat(struct TuxProc* p, int dirfd, asuserptr_t pathp, int flags, int mode);

ssize_t sys_writev(struct TuxProc* p, int fd, uintptr_t iovp, ssize_t iovcnt);

int sys_uname(struct TuxProc* p, asuserptr_t bufp);

ssize_t sys_getrandom(struct TuxProc* p, asuserptr_t bufp, size_t buflen, unsigned int flags);

typedef int tux_clockid_t;

int sys_clock_gettime(struct TuxProc* p, tux_clockid_t clockid, asuserptr_t tp);
