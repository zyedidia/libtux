#pragma once

#include <string.h>
#include <errno.h>

#include "engine.h"
#include "print.h"
#include "tux_pal.h"
#include "proc.h"

enum {
    TUX_EPERM  = 1,
    TUX_ENOENT = 2,
    TUX_EBADF  = 9,
    TUX_EACCES = 13,
    TUX_EFAULT = 14,
    TUX_EINVAL = 22,
    TUX_EMFILE = 24,
    TUX_ENOSYS = 38,
};

enum {
    TUX_MAP_PRIVATE   = 2,
    TUX_MAP_FIXED     = 16,
    TUX_MAP_ANONYMOUS = 32,
    TUX_MAP_DENYWRITE = 2048,
    TUX_MAP_NORESERVE = 16384,
};

enum {
    TUX_PROT_READ  = 1,
    TUX_PROT_WRITE = 2,
    TUX_PROT_EXEC  = 4,
};

enum {
    TUX_SEEK_SET  = 0,
    TUX_SEEK_CUR  = 1,
    TUX_SEEK_END  = 2,
};

enum {
    TUX_O_RDONLY  = 0x0,
    TUX_O_WRONLY  = 0x1,
    TUX_O_RDWR    = 0x2,
    TUX_O_CREAT   = 0x40,
    TUX_O_TRUNC   = 0x200,
    TUX_O_APPEND  = 0x400,
    TUX_O_CLOEXEC = 0x80000,
};

enum {
    TUX_AT_FDCWD      = -100,
    TUX_AT_EMPTY_PATH = 0x1000,
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

static inline int
palprot(int prot)
{
    return ((prot & TUX_PROT_READ) ? PAL_PROT_READ : 0) |
        ((prot & TUX_PROT_WRITE) ? PAL_PROT_WRITE : 0) |
        ((prot & TUX_PROT_EXEC) ? PAL_PROT_EXEC : 0);
}

static inline int
palflags(int flags)
{
    return ((flags & TUX_MAP_ANONYMOUS) ? PAL_MAP_ANONYMOUS : 0) |
        ((flags & TUX_MAP_PRIVATE) ? PAL_MAP_PRIVATE : 0);
}

static inline int
syserr(int val)
{
    if (val == -1)
        return -errno;
    return val;
}

ssize_t sys_write(struct TuxProc* p, int fd, asuserptr_t bufp, size_t size);

void sys_exit(struct TuxProc* p, int val);

uintptr_t sys_brk(struct TuxProc* p, asuserptr_t addr);

int sys_openat(struct TuxProc* p, int dirfd, asuserptr_t pathp, int flags, int mode);

ssize_t sys_writev(struct TuxProc* p, int fd, asuserptr_t iovp, size_t iovcnt);

int sys_uname(struct TuxProc* p, asuserptr_t bufp);

ssize_t sys_getrandom(struct TuxProc* p, asuserptr_t bufp, size_t buflen, unsigned int flags);

typedef int tux_clockid_t;

int sys_clock_gettime(struct TuxProc* p, tux_clockid_t clockid, asuserptr_t tp);

void sys_exit_group(struct TuxProc* p, int code);

int sys_ioctl(struct TuxProc* p, int fd, unsigned long request, uintptr_t va0,
        uintptr_t va1, uintptr_t va2, uintptr_t va3);

uintptr_t sys_mmap(struct TuxProc* p, asuserptr_t addrup, size_t length,
        int prot, int flags, int fd, off_t off);

int sys_mprotect(struct TuxProc* p, asuserptr_t addrup, size_t length, int prot);

int sys_munmap(struct TuxProc* p, asuserptr_t addrup, size_t length);

ssize_t sys_readlinkat(struct TuxProc* p, int dirfd, asuserptr_t pathp, asuserptr_t bufp, size_t size);

ssize_t sys_readlink(struct TuxProc* p, asuserptr_t pathp, asuserptr_t bufp, size_t size);

int sys_close(struct TuxProc* p, int fd);

ssize_t sys_read(struct TuxProc* p, int fd, asuserptr_t bufp, size_t size);

ssize_t sys_readv(struct TuxProc* p, int fd, asuserptr_t iovp, size_t iovcnt);

ssize_t sys_pread64(struct TuxProc* p, int fd, asuserptr_t bufp, size_t size, ssize_t offset);

int sys_newfstatat(struct TuxProc* p, int dirfd, asuserptr_t pathp, asuserptr_t statbufp, int flags);
