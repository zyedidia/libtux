#include "tux_arch.h"
#include "platform.h"

void
pal_regs_init(struct TuxRegs* regs, struct PlatAddrSpace* as, struct PlatContext* ctx)
{
    regs->x21 = as->base;
    regs->x18 = as->base;
    regs->sp = as->base;
    regs->x30 = as->base;
    regs->x25 = (uintptr_t) ctx->sys;
}
