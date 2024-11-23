#pragma once

#include "tux_pal.h"
#include "proc.h"
#include "syscalls/syscalls.h"

uintptr_t syshandle(struct TuxProc* proc, uintptr_t sysno, uintptr_t a0,
        uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5);
