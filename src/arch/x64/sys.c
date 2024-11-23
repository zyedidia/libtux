#include <stdint.h>

#include "tux_pal.h"
#include "sys.h"

#include "arch_sys.h"

void
arch_syshandle(struct PlatContext* ctx)
{
    struct TuxProc* proc = (struct TuxProc*) pal_ctx_data(ctx);
    struct TuxRegs* regs = pal_ctx_regs(ctx);

    // Platform-specific syscalls.

    // Generic syscalls.
    regs->rax = syshandle(proc, regs->rax, regs->rdi, regs->rsi, regs->rdx,
            regs->r10, regs->r8, regs->r9);
}
