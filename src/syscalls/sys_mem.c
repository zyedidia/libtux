#include <assert.h>

#include "align.h"
#include "syscalls/syscalls.h"

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
            asptr_t next = ceilp(p->brkbase + p->brksize, p->tux->opts.pagesize);
            map = pal_as_mapat(p->p_as, next, newsize - p->brksize, mapprot, mapflags, -1, 0);
        }
        if (map == (asptr_t) -1)
            return -1;
    }
    p->brksize = newsize;
    return procuseraddr(p, p->brkbase + p->brksize);
}

uintptr_t
sys_mmap(struct TuxProc* p, asuserptr_t addrup, size_t length, int prot, int flags, int fd, off_t off)
{
    if (length == 0)
        return -TUX_EINVAL;
    length = ceilp(length, p->tux->opts.pagesize);

    const int illegal_mask = ~TUX_MAP_ANONYMOUS &
        ~TUX_MAP_PRIVATE &
        ~TUX_MAP_NORESERVE &
        ~TUX_MAP_DENYWRITE &
        ~TUX_MAP_FIXED;
    if ((flags & illegal_mask) != 0) {
        VERBOSE(p->tux, "invalid mmap flag: not one of MAP_ANONYMOUS, MAP_PRIVATE, MAP_FIXED");
        return -TUX_EINVAL;
    }

    asuserptr_t i_addrp = addrup;

    int r;
    asptr_t addrp;
    if ((flags & TUX_MAP_FIXED) != 0) {
        addrup = truncp(addrup, p->tux->opts.pagesize);
        addrp = procaddr(p, addrup);
        r = procmapat(p, addrp, length, prot, flags, fd, off);
    } else {
        r = procmapany(p, length, prot, flags, fd, off, &addrp);
    }
    if (r < 0) {
        VERBOSE(p->tux, "sys_mmap(%lx (%lx), %ld, %d, %d, %d, %ld) = %d", addrp, i_addrp, length, prot, flags, fd, off, r);
        return r;
    }
    asuserptr_t ret = procuseraddr(p, addrp);
    VERBOSE(p->tux, "sys_mmap(%lx (%lx), %ld, %d, %d, %d, %ld) = %lx", addrp, i_addrp, length, prot, flags, fd, off, ret);
    return ret;
}

int
sys_mprotect(struct TuxProc* p, asuserptr_t addrup, size_t length, int prot)
{
    asptr_t addrp = procaddr(p, addrup);
    return pal_as_mprotect(p->p_as, addrp, length, palprot(prot));
}

int
sys_munmap(struct TuxProc* p, asuserptr_t addrup, size_t length)
{
    return procunmap(p, procaddr(p, addrup), length);
}
