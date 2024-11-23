// For REG_XXX macros
#define _GNU_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

#include "tux_pal.h"
#include "platform.h"

#define SA_RESTORER 0x04000000

struct k_sigaction {
    void (*handler)(int);
    unsigned long flags;
    void (*restorer)(void);
    unsigned mask[2];
};

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

    assert(pal_myctx->plat->syshandler && "platform has no syshandler!");

    ucontext_t* uctx = (ucontext_t*) context;
#ifdef __x86_64__
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
    };
#endif

    pal_myctx->regs = regs;

    pal_myctx->plat->syshandler(pal_myctx);

    sud_block();
}

extern void sud_rt_restore(void);
extern size_t sud_rt_restore_size;

bool
sud_init(void)
{
    struct k_sigaction sa;
    sa.flags = SA_SIGINFO | SA_RESTORER;
    sa.handler = (void*) &handle_sigsys;
    sa.restorer = sud_rt_restore;
    sigemptyset((sigset_t*) &sa.mask);
    uint64_t r = syscall(SYS_rt_sigaction, SIGSYS, &sa, 0, _NSIG/8);
    if (r != 0) {
        fprintf(stderr, "error setting sigaction: %ld\n", r);
        return false;
    }

    // Enable SUD.
    if (prctl(PR_SET_SYSCALL_USER_DISPATCH, PR_SYS_DISPATCH_ON, sud_rt_restore, sud_rt_restore_size, &filter) != 0) {
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
