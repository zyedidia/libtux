#pragma once

#include "tux_pal.h"

enum {
    TUX_SYS_write = 1,
    TUX_SYS_exit  = 60,
};

void arch_syshandle(struct PlatContext* ctx);
