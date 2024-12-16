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
        sys = mmap((void*) base, plat->opts.pagesize, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
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
syssetup(struct Sys* sys, struct PlatContext* ctx, uintptr_t base)
{
    sys->rtcalls[0] = (uintptr_t) &pal_syscall_entry;
    sys->rtcalls[1] = (uintptr_t) &pal_get_tp;
    sys->rtcalls[2] = (uintptr_t) &pal_set_tp;
    sys->base = base;
    sys->ctx = (uintptr_t) ctx;
}

struct PlatContext*
pal_ctx_new(struct Platform* plat, struct PlatAddrSpace* as, void* ctxp)
{
    struct PlatContext* ctx = malloc(sizeof(struct PlatContext));
    if (!ctx)
        return NULL;

    struct Sys* sys = sysalloc(plat, as->base);
    if (!sys)
        assert(!"unimplemented");
    syssetup(sys, ctx, as->base);

    *ctx = (struct PlatContext) {
        .ctxp = ctxp,
        .plat = plat,
        .sys = sys,
    };

    pal_regs_init(&ctx->regs, as, ctx);

    return ctx;
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
    assert(!"unimplemented");
}
