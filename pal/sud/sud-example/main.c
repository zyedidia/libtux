#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/prctl.h>

extern char syscall_start;
extern char syscall_end;

extern void rt_restore(void);
extern size_t rt_restore_size;

typedef uint64_t (*SyscallFn)(uint64_t num, uint64_t a, uint64_t b, uint64_t c,
        uint64_t d, uint64_t e);

static volatile char filter = SYSCALL_DISPATCH_FILTER_BLOCK;

static void handle_sigsys(int signo, siginfo_t *info, void *context)
{
    filter = SYSCALL_DISPATCH_FILTER_ALLOW;

    ucontext_t *ctx = (ucontext_t *)context;
#ifdef __x86_64__
    fprintf(stderr, "syscall: rax = %016llx, rdi = %016llx, rsi = %016llx, rdx = %016llx\n",
        ctx->uc_mcontext.gregs[REG_RAX],
        ctx->uc_mcontext.gregs[REG_RDI],
        ctx->uc_mcontext.gregs[REG_RSI],
        ctx->uc_mcontext.gregs[REG_RDX]);

    if (ctx->uc_mcontext.gregs[REG_RAX] == 1ull) {
        fprintf(stderr, "write('%.*s')\n",
            (int)ctx->uc_mcontext.gregs[REG_RDX],
            (char *)ctx->uc_mcontext.gregs[REG_RSI]);
        ctx->uc_mcontext.gregs[REG_RAX] = ctx->uc_mcontext.gregs[REG_RDX] + 1;
    }
#else
#pragma message "Unsupported architecture. No information will be printed for syscalls."
#endif

    filter = SYSCALL_DISPATCH_FILTER_BLOCK;
}

#define SA_RESTORER 0x04000000

struct k_sigaction {
    void (*handler)(int);
    unsigned long flags;
    void (*restorer)(void);
    unsigned mask[2];
};

int main() {
    printf("entered main\n");

    struct k_sigaction sa;
    sa.flags = SA_SIGINFO | SA_RESTORER;
    sa.handler = (void*) &handle_sigsys;
    sa.restorer = rt_restore;
    sigemptyset((sigset_t*) &sa.mask);
    uint64_t r = syscall(SYS_rt_sigaction, SIGSYS, &sa, 0, _NSIG/8);
    if (r != 0) {
        printf("error setting sigaction\n");
        return 1;
    }

    size_t size = &syscall_end - &syscall_start;
    void* p = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == (void*) -1) {
        perror("mmap");
    }

    memcpy(p, &syscall_start, size);

    SyscallFn user_syscall = (SyscallFn) p;

    prctl(PR_SET_SYSCALL_USER_DISPATCH, PR_SYS_DISPATCH_ON, rt_restore, rt_restore_size, &filter);

    user_syscall(SYS_write, 1, (uint64_t) "hello\n", 6, 0, 0);
    user_syscall(SYS_write, 1, (uint64_t) "hello\n", 6, 0, 0);
    user_syscall(SYS_write, 1, (uint64_t) "hello\n", 6, 0, 0);

    filter = SYSCALL_DISPATCH_FILTER_ALLOW;

    return 0;
}
