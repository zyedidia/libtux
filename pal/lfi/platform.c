#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include "tux_pal.h"
#include "platform.h"

struct Platform*
lfi_new_plat(size_t pagesize)
{
    struct PlatOptions opts = (struct PlatOptions) {
        .pagesize = pagesize,
        .vmsize = gb(4),
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

void
pal_sys_handler(struct Platform* plat, SysHandlerFn fn)
{
    plat->syshandler = fn;
}

void pal_syscall_handler(struct PlatContext* ctx)
    asm ("pal_syscall_handler");

void
pal_syscall_handler(struct PlatContext* ctx)
{
    assert(ctx->plat->syshandler && "platform does not have a system call handler");
    ctx->plat->syshandler(ctx);
}
