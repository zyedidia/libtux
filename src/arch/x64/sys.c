#include <assert.h>
#include <stdint.h>

#include "tux_pal.h"
#include "sys.h"
#include "syscalls/strace.h"
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

static const char*
arch_sysname(uint64_t sysno)
{
    switch (sysno) {
    STRACE_CASE(arch_prctl)
    STRACE_CASE(readlink)
    STRACE_CASE(access)
    }
    return NULL;
}

void
arch_syshandle(struct PlatContext* ctx)
{
    struct TuxProc* proc = (struct TuxProc*) pal_ctx_data(ctx);
    struct TuxRegs* regs = pal_ctx_regs(ctx);

    uint64_t orig_rax = regs->rax;

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
    case TUX_SYS_unlink:
        regs->rax = sys_unlink(proc, regs->rdi);
        break;
    case TUX_SYS_time:
        regs->rax = sys_time(proc, regs->rdi);
        break;
    case TUX_SYS_chown:
        regs->rax = sys_chown(proc, regs->rdi, regs->rsi, regs->rdx);
        break;
    case TUX_SYS_chmod:
        regs->rax = sys_chmod(proc, regs->rdi, regs->rsi);
        break;
    case TUX_SYS_rename:
        regs->rax = sys_rename(proc, regs->rdi, regs->rsi);
        break;
    default:
        // Generic syscalls.
        regs->rax = syshandle(proc, regs->rax, regs->rdi, regs->rsi, regs->rdx,
                regs->r10, regs->r8, regs->r9);
    }

    if (proc->tux->opts.strace) {
        const char* name = arch_sysname(orig_rax);
        if (!name)
            name = sysname(orig_rax);
        fprintf(stderr, "strace: %s = %lx\n", name, regs->rax);
    }
}
