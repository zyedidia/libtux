#include <assert.h>
#include <stdlib.h>

#include <immintrin.h>

#include "platform.h"

// TODO: make this thread-local with TID.
struct PlatContext* pal_myctx;

extern uint64_t pal_ctx_entry(struct PlatContext* ctx, void** kstackp)
    asm ("pal_ctx_entry");

extern void pal_asm_ctx_exit(void* kstackp, uint64_t val)
    asm ("pal_asm_ctx_exit");

struct PlatContext*
pal_ctx_new(struct Platform* plat, void* ctxp)
{
    struct PlatContext* ctx = malloc(sizeof(struct PlatContext));
    if (!ctx)
        return NULL;
    *ctx = (struct PlatContext) {
        .ctxp = ctxp,
        .plat = plat,
        .ktp = _readfsbase_u64(),
    };
    return ctx;
}

uint64_t
pal_ctx_run(struct PlatContext* ctx, struct PlatAddrSpace* as)
{
    (void) as;
    pal_myctx = ctx;

    sud_block();
    uint64_t ret = pal_ctx_entry(ctx, &ctx->kstackp);
    sud_allow();

    return ret;
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

void*
pal_ctx_data(struct PlatContext* ctx)
{
    return ctx->ctxp;
}

void
pal_ctx_exit(struct PlatContext* ctx, uint64_t val)
{
    pal_myctx = NULL;
    pal_asm_ctx_exit(ctx->kstackp, val);
}

void
pal_ctx_tpset(struct PlatContext* ctx, asptr_t tp)
{
    ctx->regs.fs = tp;
}
