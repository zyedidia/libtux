#pragma once

#include <stddef.h>
#include <stdint.h>

struct TuxOptions {
    size_t pagesize;
    size_t stacksize;
};

struct Platform;

struct Tux {
    struct Platform* plat;
    struct TuxOptions opts;
};

struct TuxProc;

struct Tux* tux_new(struct Platform* plat, struct TuxOptions opts);

struct TuxProc* tux_proc_newfile(struct Tux* tux, uint8_t* prog, size_t progsize, int argc, char** argv);

void tux_proc_start(struct Tux* tux, struct TuxProc* p);
