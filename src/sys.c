#include <assert.h>

#include "sys.h"
#include "syscalls/strace.h"
#include "arch_sys.h"

#define SYS(SYSNO, expr)  \
    case TUX_SYS_##SYSNO: \
        r = expr;         \
        break;

uintptr_t
syshandle(struct TuxProc* proc, uintptr_t sysno, uintptr_t a0, uintptr_t a1,
        uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5)
{
    uintptr_t r = -TUX_ENOSYS;
    switch (sysno) {
    SYS(gettid,          0)
    SYS(getpid,          0)
    SYS(write,           sys_write(proc, a0, a1, a2))
    SYS(read,            sys_read(proc, a0, a1, a2))
    SYS(lseek,           sys_lseek(proc, a0, a1, a2))
    SYS(exit,            sys_exit(proc, a0))
    SYS(exit_group,      sys_exit_group(proc, a0))
    SYS(brk,             sys_brk(proc, a0))
    SYS(openat,          sys_openat(proc, a0, a1, a2, a3))
    SYS(close,           sys_close(proc, a0))
    SYS(writev,          sys_writev(proc, a0, a1, a2))
    SYS(readv,           sys_readv(proc, a0, a1, a2))
    SYS(pread64,         sys_pread64(proc, a0, a1, a2, a3))
    SYS(mmap,            sys_mmap(proc, a0, a1, a2, a3, a4, a5))
    SYS(mprotect,        sys_mprotect(proc, a0, a1, a2))
    SYS(munmap,          sys_munmap(proc, a0, a1))
    SYS(getdents64,      sys_getdents64(proc, a0, a1, a2))
    SYS(newfstatat,      sys_newfstatat(proc, a0, a1, a2, a3))
    SYS(getrandom,       sys_getrandom(proc, a0, a1, a2))
    SYS(clock_gettime,   sys_clock_gettime(proc, a0, a1))
    SYS(ioctl,           sys_ioctl(proc, a0, a1, a2, a3, a4, a5))
    SYS(futex,           sys_futex(proc, a0, a1, a2, a3, a4, a5))
    SYS(clone,           sys_clone(proc, a0, a1, a2, a3, a4, a5))
    SYS(rt_sigaction,    sys_rt_sigaction(proc, a0, a1, a2, a3))
    SYS(rt_sigprocmask,  sys_rt_sigprocmask(proc, a0, a1, a2, a3))
    SYS(rt_sigreturn,    sys_rt_sigreturn(proc))
    SYS(set_tid_address, 0)
    SYS(set_robust_list, 0)
    SYS(uname,           sys_uname(proc, a0))
    SYS(statx,           -TUX_ENOSYS)
    SYS(rseq,            -TUX_ENOSYS)
    SYS(prlimit64,       -TUX_ENOSYS)
    SYS(statfs,          -TUX_ENOSYS)
    SYS(poll,            -TUX_ENOSYS)
    SYS(getxattr,        -TUX_ENOSYS)
    SYS(lgetxattr,       -TUX_ENOSYS)
    SYS(socket,          -TUX_ENOSYS)
    default:
        fprintf(stderr, "unknown syscall: %ld (%s)\n", sysno, sysname(sysno));
        assert(!"unhandled syscall");
    }

    return r;
}
