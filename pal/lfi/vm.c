#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

#include "tux_pal.h"
#include "boxmap.h"
#include "platform.h"

static size_t
guardsize(void)
{
    return 80 * 1024;
}

struct PlatAddrSpace*
pal_as_new(struct Platform* plat)
{
    struct PlatAddrSpace* as = malloc(sizeof(struct PlatAddrSpace));
    if (!as)
        return NULL;
    size_t size = plat->opts.vmsize;
    uintptr_t base = boxmap_addspace(plat->bm, size);
    if (base == 0)
        goto err1;

    *as = (struct PlatAddrSpace) {
        .base = base,
        .size = size,
        .minaddr = base + guardsize() + plat->opts.pagesize, // for sys page
        .maxaddr = base + size - guardsize(),
    };
    bool ok = mm_init(&as->mm, as->minaddr, as->maxaddr - as->minaddr, plat->opts.pagesize);
    if (!ok)
        goto err2;

    return as;

err2:
    boxmap_rmspace(plat->bm, base, size);
err1:
    free(as);
    return NULL;
}

struct TuxAddrSpaceInfo
pal_as_info(struct PlatAddrSpace* as)
{
    return (struct TuxAddrSpaceInfo) {
        .base = as->base,
        .size = as->size,
        .minaddr = as->minaddr,
        .maxaddr = as->maxaddr,
    };
}

static int
asmap(struct PlatAddrSpace* as, uintptr_t start, size_t size, int prot,
        int flags, struct HostFile* hf, off_t off)
{
    // TODO: verify
    void* mem = host_mmap((void*) start, size, prot, flags | MAP_FIXED, hf, off);
    if (mem == (void*) -1)
        return -errno;
    return 0;
}

asptr_t
pal_as_mapany(struct PlatAddrSpace* as, size_t size, int prot, int flags,
        struct HostFile* hf, off_t off)
{
    asptr_t addr = mm_mapany(&as->mm, size, prot, flags, hf, off);
    if (addr == (asptr_t) -1)
        return 0;
    int r = asmap(as, addr, size, prot, flags, hf, off);
    if (r < 0) {
        mm_unmap(&as->mm, addr, size);
        return (asptr_t) -1;
    }
    return addr;
}

static void
cbunmap(uint64_t start, size_t len, MMInfo info, void* udata)
{
    (void) udata, (void) info;
    void* p = host_mmap((void*) start, len, TUX_PROT_NONE, TUX_MAP_ANONYMOUS |
            TUX_MAP_PRIVATE | TUX_MAP_FIXED, NULL, 0);
    assert(p == (void*) start);
}

asptr_t
pal_as_mapat(struct PlatAddrSpace* as, asptr_t addr, size_t size, int prot,
        int flags, struct HostFile* hf, off_t off)
{
    assert(addr >= as->minaddr && addr + size <= as->maxaddr);

    addr = (asptr_t) mm_mapat_cb(&as->mm, addr, size, prot, flags, hf, off, cbunmap, NULL);
    if (addr == (asptr_t) -1)
        return (asptr_t) -1; 
    int r = asmap(as, addr, size, prot, flags, hf, off);
    if (r < 0) {
        mm_unmap(&as->mm, addr, size);
        return (asptr_t) -1;
    }
    return addr;
}

int
pal_as_mprotect(struct PlatAddrSpace* as, asptr_t addr, size_t size, int prot)
{
    assert(addr >= as->minaddr && addr + size <= as->maxaddr);

    // TODO: verify

    // TODO: mark the mapping with libmmap?

    return host_mprotect((void*) addr, size, prot);
}

int
pal_as_munmap(struct PlatAddrSpace* as, asptr_t addr, size_t size)
{
    if (addr >= as->minaddr && addr + size < as->maxaddr)
        return mm_unmap_cb(&as->mm, addr, size, cbunmap, NULL);
    return -1;
}

void
pal_as_free(struct PlatAddrSpace* as)
{
    assert(!"unimplemented");
}

asuserptr_t
pal_as_p2user(struct PlatAddrSpace* as, void* p)
{
    asuserptr_t userp = (asuserptr_t) p;
    assert(userp >= as->minaddr && userp < as->maxaddr);
    return userp;
}

void*
pal_as_user2p(struct PlatAddrSpace* as, asuserptr_t userp)
{
    if (userp >= as->minaddr && userp < as->maxaddr)
        return (void*) userp;
    return NULL;
}

asuserptr_t
pal_as_ap2user(struct PlatAddrSpace* as, asptr_t asp)
{
    return (asuserptr_t) asp;
}

asptr_t
pal_as_user2ap(struct PlatAddrSpace* as, asuserptr_t asup)
{
    return (asptr_t) asup;
}
