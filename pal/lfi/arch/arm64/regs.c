#include "tux_arch.h"
#include "platform.h"
#include "arch/arm64/regs.h"

void
pal_regs_init(struct TuxRegs* regs, struct PlatAddrSpace* as, struct PlatContext* ctx)
{
    regs->REG_BASE = as->base;
    regs->REG_ADDR = as->base;
    regs->sp = as->base;
    regs->x30 = as->base;
    regs->REG_SYS = (uintptr_t) ctx->sys;
}
