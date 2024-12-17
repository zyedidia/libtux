#include "tux_arch.h"
#include "platform.h"

void
pal_regs_init(struct TuxRegs* regs, struct PlatAddrSpace* as, struct PlatContext* ctx)
{
    regs->r14 = as->base;
    regs->gs  = as->base;
    regs->rsp = as->base;
    regs->r13 = (uintptr_t) ctx->sys;
}
