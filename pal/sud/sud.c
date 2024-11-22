#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "tux_pal.h"
#include "boxmap.h"
#include "platform.h"

static size_t
gb(size_t x)
{
    return x * 1024 * 1024 * 1024;
}

struct Platform*
sud_new_plat(void)
{
    struct PlatOptions opts = (struct PlatOptions) {
        .pagesize = getpagesize(),
        .vmsize = 4ULL * 1024 * 1024 * 1024,
    };
    struct Platform* plat = malloc(sizeof(struct Platform));
    if (!plat)
        return NULL;

    struct BoxMap* bm = boxmap_new((struct BoxMapOptions) {
        .minalign = gb(4),
        .maxalign = gb(4),
        .guardsize = 0,
    });
    if (!bm)
        goto err1;
    if (!boxmap_reserve(bm, gb(256)))
        goto err2;

    *plat = (struct Platform) {
        .bm = bm,
        .opts = opts,
    };
    return plat;

err2:
    boxmap_delete(bm);
err1:
    free(plat);
    return NULL;
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
        .minaddr = base,
        .maxaddr = base + size,
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

struct PlatContext*
pal_ctx_new(struct Platform* plat, void* ctxp)
{
    assert(!"unimplemented");
}

asptr_t
pal_as_mmap(struct PlatAddrSpace* as, asptr_t addr, size_t size, int prot, int flags, int fd, off_t off)
{
    assert(!"unimplemented");
}

int
pal_as_mprotect(struct PlatAddrSpace* as, asptr_t addr, size_t size, int prot)
{
    assert(!"unimplemented");
}

int
pal_as_munmap(struct PlatAddrSpace* as, asptr_t addr, size_t size)
{
    assert(!"unimplemented");
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
