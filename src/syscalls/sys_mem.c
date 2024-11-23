#include <assert.h>

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
            // First unmap where we want to expand to, and then mm
            map = pal_as_mapat(p->p_as, p->brkbase + p->brksize, newsize - p->brksize, mapprot, mapflags, -1, 0);
        }
        if (map == (asptr_t) -1)
            return -1;
    }
    p->brksize = newsize;
    return procuseraddr(p, p->brkbase + p->brksize);
}
