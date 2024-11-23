// For REG_XXX macros
#define _GNU_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

#include <immintrin.h>

#include "tux_pal.h"
#include "platform.h"

extern void pal_sigsys_return(struct PlatContext* ctx)
    asm ("pal_sigsys_return");

static volatile char filter = SYSCALL_DISPATCH_FILTER_ALLOW;

void
sud_block(void)
{
    filter = SYSCALL_DISPATCH_FILTER_BLOCK;
}

void
sud_allow(void)
{
    filter = SYSCALL_DISPATCH_FILTER_ALLOW;
}

static void
handle_sigsys(int signo, siginfo_t* info, void* context)
{
    sud_allow();

    uint64_t gs = _readgsbase_u64();
    uint64_t fs = _readfsbase_u64();

    _writefsbase_u64(pal_myctx->ktp);

    assert(pal_myctx->plat->syshandler && "platform has no syshandler!");

    ucontext_t* uctx = (ucontext_t*) context;
    struct TuxRegs regs = (struct TuxRegs) {
        .rsp = uctx->uc_mcontext.gregs[REG_RSP],
        .rax = uctx->uc_mcontext.gregs[REG_RAX],
        .rcx = uctx->uc_mcontext.gregs[REG_RCX],
        .rdx = uctx->uc_mcontext.gregs[REG_RDX],
        .rbx = uctx->uc_mcontext.gregs[REG_RBX],
        .rbp = uctx->uc_mcontext.gregs[REG_RBP],
        .rsi = uctx->uc_mcontext.gregs[REG_RSI],
        .rdi = uctx->uc_mcontext.gregs[REG_RDI],
        .r8 = uctx->uc_mcontext.gregs[REG_R8],
        .r9 = uctx->uc_mcontext.gregs[REG_R9],
        .r10 = uctx->uc_mcontext.gregs[REG_R10],
        .r11 = uctx->uc_mcontext.gregs[REG_R11],
        .r12 = uctx->uc_mcontext.gregs[REG_R12],
        .r13 = uctx->uc_mcontext.gregs[REG_R13],
        .r14 = uctx->uc_mcontext.gregs[REG_R14],
        .r15 = uctx->uc_mcontext.gregs[REG_R15],
        .fs = fs,
        .gs = gs,
    };
    // rcx is clobbered in the syscall abi so we can put the return address
    // there.
    regs.rcx = uctx->uc_mcontext.gregs[REG_RIP];

    pal_myctx->regs = regs;

    pal_myctx->plat->syshandler(pal_myctx);

    sud_block();

    // Does not return.
    pal_sigsys_return(pal_myctx);
}

bool
sud_init(void)
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sa.sa_sigaction = &handle_sigsys;
    sigemptyset((sigset_t*) &sa.sa_mask);
    int r = sigaction(SIGSYS, &sa, NULL);
    if (r != 0) {
        fprintf(stderr, "siaction error: %s\n", strerror(errno));
        return false;
    }

    // Enable SUD.
    if (prctl(PR_SET_SYSCALL_USER_DISPATCH, PR_SYS_DISPATCH_ON, 0, 0, &filter) != 0) {
        fprintf(stderr, "prctl error: %s\n", strerror(errno));
        return false;
    }

    return true;
}

void
pal_sys_handler(struct Platform* plat, SysHandlerFn fn)
{
    plat->syshandler = fn;
}
