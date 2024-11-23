#include <assert.h>

#include "sys.h"
#include "arch_sys.h"

uintptr_t
syshandle(struct TuxProc* proc, uintptr_t sysno, uintptr_t a0, uintptr_t a1,
        uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5)
{
    uintptr_t r = -1;
    switch (sysno) {
    case TUX_SYS_write:
        r = sys_write(proc, a0, a1, a2);
        break;
    case TUX_SYS_exit:
        sys_exit(proc, a0);
        break;
    case TUX_SYS_exit_group:
        sys_exit_group(proc, a0);
        break;
    case TUX_SYS_brk:
        r = sys_brk(proc, a0);
        break;
    case TUX_SYS_openat:
        r = sys_openat(proc, a0, a1, a2, a3);
        break;
    case TUX_SYS_writev:
        r = sys_writev(proc, a0, a1, a2);
        break;
    case TUX_SYS_getrandom:
        r = sys_getrandom(proc, a0, a1, a2);
        break;
    case TUX_SYS_clock_gettime:
        r = sys_clock_gettime(proc, a0, a1);
        break;
    case TUX_SYS_ioctl:
        r = sys_ioctl(proc, a0, a1, a2, a3, a4, a5);
        break;
    case TUX_SYS_set_tid_address:
        r = 0;
        break;
    case TUX_SYS_set_robust_list:
        r = 0;
        break;
    case TUX_SYS_rseq:
        r = -TUX_ENOSYS;
        break;
    case TUX_SYS_uname:
        r = sys_uname(proc, a0);
        break;
    case TUX_SYS_prlimit64:
        r = -TUX_ENOSYS;
        break;
    case TUX_SYS_readlink:
        r = -TUX_ENOSYS;
        break;
    default:
        fprintf(stderr, "unknown syscall: %ld\n", sysno);
        assert(!"unhandled syscall");
    }
    return r;
}
