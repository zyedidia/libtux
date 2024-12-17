#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct TuxOptions {
    size_t pagesize;
    size_t stacksize;
    bool verbose;
    bool strace;
    bool perf;
};

struct Platform;

struct Tux;

struct TuxProc;

struct Tux* tux_new(struct Platform* plat, struct TuxOptions opts);

struct TuxThread* tux_proc_newfile(struct Tux* tux, uint8_t* prog, size_t progsize, int argc, char** argv);

uint64_t tux_proc_start(struct Tux* tux, struct TuxThread* p);
