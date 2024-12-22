#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "platform.h"
#include "regs.h"

_Thread_local struct PlatContext* pal_myctx;

#define asm __asm__

extern uint64_t pal_ctx_entry(struct PlatContext* ctx, void** kstackp)
    asm ("pal_ctx_entry");

extern void pal_asm_ctx_exit(void* kstackp, uint64_t val)
    asm ("pal_asm_ctx_exit");

static struct Sys*
sysalloc(struct Platform* plat, uintptr_t base)
{
    struct Sys* sys;
    if (plat->opts.sysexternal) {
        sys = malloc(sizeof(struct Sys));
        if (!sys)
            return NULL;
    } else {
        sys = host_mmap((void*) base, plat->opts.pagesize, TUX_PROT_READ | TUX_PROT_WRITE,
                TUX_MAP_PRIVATE | TUX_MAP_ANONYMOUS | TUX_MAP_FIXED, NULL, 0);
        if (sys == (void*) -1)
            return NULL;
    }
    return sys;
}

extern void pal_syscall_entry(void)
    asm ("pal_syscall_entry");
extern void pal_get_tp(void)
    asm ("pal_get_tp");
extern void pal_set_tp(void)
    asm ("pal_set_tp");

static void
syssetup(struct Platform* plat, struct Sys* sys, struct PlatContext* ctx, uintptr_t base)
{
    sys->rtcalls[0] = (uintptr_t) &pal_syscall_entry;
    sys->rtcalls[1] = (uintptr_t) &pal_get_tp;
    sys->rtcalls[2] = (uintptr_t) &pal_set_tp;
    sys->base = base;
    // Only used in sysexternal mode (where there is a syspage per context)
    sys->ctx = (uintptr_t) ctx;
    int err = host_mprotect((void*) base, plat->opts.pagesize, TUX_PROT_READ);
    assert(err == 0);
}

struct PlatContext*
pal_ctx_new(struct Platform* plat, struct PlatAddrSpace* as, void* ctxp, bool main)
{
    struct PlatContext* ctx = malloc(sizeof(struct PlatContext));
    if (!ctx)
        return NULL;

    struct Sys* sys;
    if (main) {
        sys = sysalloc(plat, as->base);
        if (!sys)
            goto err;
        syssetup(plat, sys, ctx, as->base);
    } else {
        sys = (struct Sys*) as->base;
    }

    *ctx = (struct PlatContext) {
        .ctxp = ctxp,
        .plat = plat,
        .sys = sys,
    };

    pal_regs_init(&ctx->regs, as, ctx);

    return ctx;
err:
    free(ctx);
    return NULL;
}

uint64_t
pal_ctx_run(struct PlatContext* ctx, struct PlatAddrSpace* as)
{
    (void) as;
    pal_myctx = ctx;

    uint64_t ret = pal_ctx_entry(ctx, &ctx->kstackp);

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
    ctx->tp = tp;
}
