#include <assert.h>
#include <stdlib.h>

#include "platform.h"

// TODO: make this thread-local with TID.
struct PlatContext* pal_myctx;

extern void pal_ctx_entry(struct PlatContext* ctx, void** kstackp)
    asm ("pal_ctx_entry");

struct PlatContext*
pal_ctx_new(struct Platform* plat, void* ctxp)
{
    struct PlatContext* ctx = malloc(sizeof(struct PlatContext));
    if (!ctx)
        return NULL;
    *ctx = (struct PlatContext) {
        .ctxp = ctxp,
        .plat = plat,
    };
    return ctx;
}

void
pal_ctx_resume(struct PlatContext* ctx, struct PlatAddrSpace* as)
{
    (void) as;
    pal_myctx = ctx;

    sud_block();
    pal_ctx_entry(ctx, &ctx->kstackp);
    sud_allow();

    pal_myctx = NULL;
}

void
pal_ctx_free(struct PlatContext* ctx)
{
    free(ctx);
}

struct TuxRegs*
pal_ctx_regs(struct PlatContext* ctx)
{
    return &ctx->regs;
}
