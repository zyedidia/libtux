#include <assert.h>

#include "arch_regs.h"

void
regs_init(struct TuxRegs* regs, asptr_t entry, asptr_t sp)
{
    regs->sp = sp;
    regs->x30 = entry;
}
