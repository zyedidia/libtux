#pragma once

#include "platform.h"

struct Tux {
    struct Platform* plat;
    struct TuxPlatformFuncs pf;
};

struct TuxProc;

struct Tux* tux_new(struct Platform* plat, struct TuxPlatformFuncs platfns);

struct TuxProc* tux_proc_new(struct Tux* tux, uint8_t* prog, size_t progsize, int argc, char** argv);

void tux_run(struct Tux* tux);
