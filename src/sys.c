#include <stdlib.h>

#include "sys.h"

void
syshandler(struct PlatContext* ctx)
{
    struct TuxRegs* regs = pal_ctx_regs(ctx);
    printf("syscall: %%rax: %lx, %%rdi: %lx\n", regs->rax, regs->rdi);

    exit(1);
}
