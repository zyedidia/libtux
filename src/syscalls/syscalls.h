#pragma once

#include <string.h>
#include <errno.h>

#include "engine.h"
#include "print.h"
#include "tux_pal.h"
#include "proc.h"

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
syserr(int val)
{
    if (val == -1)
        return -errno;
    return val;
}

ssize_t sys_write(struct TuxProc* p, int fd, asuserptr_t bufp, size_t size);

uintptr_t sys_exit(struct TuxProc* p, int val);

uintptr_t sys_brk(struct TuxProc* p, asuserptr_t addr);

int sys_openat(struct TuxProc* p, int dirfd, asuserptr_t pathp, int flags, int mode);

ssize_t sys_writev(struct TuxProc* p, int fd, asuserptr_t iovp, size_t iovcnt);

int sys_uname(struct TuxProc* p, asuserptr_t bufp);

ssize_t sys_getrandom(struct TuxProc* p, asuserptr_t bufp, size_t buflen, unsigned int flags);

typedef int tux_clockid_t;

int sys_clock_gettime(struct TuxProc* p, tux_clockid_t clockid, asuserptr_t tp);

uintptr_t sys_exit_group(struct TuxProc* p, int code);

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

ssize_t sys_getdents64(struct TuxProc* p, int fd, asuserptr_t dirp, size_t count);

off_t sys_lseek(struct TuxProc* p, int fd, off_t offset, int whence);

long sys_futex(struct TuxProc* p, asuserptr_t uaddrp, int op, uint32_t val, uint64_t timeoutp, asuserptr_t uaddr2p, uint32_t val3);

int sys_clone(struct TuxProc* p, uint64_t flags, uint64_t stack, uint64_t ptid, uint64_t ctid, uint64_t tls, uint64_t func);
