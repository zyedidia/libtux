#include <assert.h>

#include "arch_regs.h"

void
regs_init(struct TuxRegs* regs, asptr_t entry, asptr_t sp)
{
    regs->rsp = sp - 8;
    // does not work for PLAT_ADDR_SPACE_EXTERNAL
    *((uint64_t*) regs->rsp) = entry;
}

uintptr_t*
regs_return(struct TuxRegs* regs)
{
    return &regs->rax;
}

uintptr_t*
regs_sp(struct TuxRegs* regs)
{
    return &regs->rsp;
}
