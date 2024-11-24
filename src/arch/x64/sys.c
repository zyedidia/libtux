#include <assert.h>
#include <stdint.h>

#include "tux_pal.h"
#include "sys.h"
#include "syscalls/syscalls.h"

#include "arch_sys.h"

enum {
    TUX_ARCH_SET_FS = 0x1002,
};

static int
sys_arch_prctl(struct TuxProc* p, int code, asuserptr_t addr)
{
    switch (code) {
    case TUX_ARCH_SET_FS:
        pal_ctx_tpset(p->p_ctx, procaddr(p, addr));
        return 0;
    default:
        return -TUX_EINVAL;
    }
}

void
arch_syshandle(struct PlatContext* ctx)
{
    struct TuxProc* proc = (struct TuxProc*) pal_ctx_data(ctx);
    struct TuxRegs* regs = pal_ctx_regs(ctx);

    switch (regs->rax) {
    case TUX_SYS_arch_prctl:
        regs->rax = sys_arch_prctl(proc, regs->rdi, regs->rsi);
        break;
    case TUX_SYS_readlink:
        regs->rax = sys_readlink(proc, regs->rdi, regs->rsi, regs->rdx);
        break;
    case TUX_SYS_access:
        regs->rax = -TUX_ENOSYS;
        break;
    default:
        // Generic syscalls.
        regs->rax = syshandle(proc, regs->rax, regs->rdi, regs->rsi, regs->rdx,
                regs->r10, regs->r8, regs->r9);
    }
}
