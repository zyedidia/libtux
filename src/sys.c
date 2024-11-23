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
        r = sys_exit(proc, a0);
        break;
    default:
        fprintf(stderr, "unknown syscall: %ld\n", sysno);
        assert(!"unhandled syscall");
    }
    return r;
}
