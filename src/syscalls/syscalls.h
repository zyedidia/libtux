#pragma once

#include "tux_pal.h"
#include "proc.h"

ssize_t sys_write(struct TuxProc* proc, int fd, asuserptr_t bufp, size_t size);

uintptr_t sys_exit(struct TuxProc* proc, uint64_t val);
