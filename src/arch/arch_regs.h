#pragma once

#include "tux_arch.h"
#include "tux_pal.h"

void regs_init(struct TuxRegs* regs, asptr_t entry, asptr_t sp);

uintptr_t* regs_return(struct TuxRegs* regs);

uintptr_t* regs_sp(struct TuxRegs* regs);
