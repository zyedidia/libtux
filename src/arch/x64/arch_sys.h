#pragma once

#include "tux_pal.h"

enum {
    TUX_SYS_write              = 1,
    TUX_SYS_brk                = 12,
    TUX_SYS_ioctl              = 16,
    TUX_SYS_writev             = 20,
    TUX_SYS_exit               = 60,
    TUX_SYS_uname              = 63,
    TUX_SYS_readlink           = 89,
    TUX_SYS_arch_prctl         = 158,
    TUX_SYS_set_tid_address    = 218,
    TUX_SYS_clock_gettime      = 228,
    TUX_SYS_exit_group         = 231,
    TUX_SYS_openat             = 257,
    TUX_SYS_set_robust_list    = 273,
    TUX_SYS_prlimit64          = 302,
    TUX_SYS_getrandom          = 318,
    TUX_SYS_rseq               = 334,
};

void arch_syshandle(struct PlatContext* ctx);
