#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "tux.h"
#include "platform.h"

enum {
    PATH_MAX = 4096,
    NOFILE = 128,
};

struct TuxProc;

struct Stat;

struct FDFile {
    void* dev;
    size_t refs;
    ssize_t (*read)(void*, struct TuxProc*, uint8_t*, size_t);
    ssize_t (*write)(void*, struct TuxProc*, uint8_t*, size_t);
    ssize_t (*lseek)(void*, struct TuxProc*, off_t, int);
    int (*close)(void*, struct TuxProc*);
    int (*stat)(void*, struct TuxProc*, struct Stat*);
    ssize_t (*getdents)(void*, struct TuxProc*, void*, size_t);
    int (*mapfd)(void*);
};

struct FDTable {
    struct FDFile* files[NOFILE];
};

struct Dir {
    char name[PATH_MAX];
    int fd;
};

struct TuxProc {
    struct PlatContext* p_ctx;
    struct PlatAddrSpace* p_as;
    struct Tux* tux;
    uintptr_t base;
    size_t size;

    struct FDTable fdtable;
    struct Dir cwd;

    uintptr_t brkbase;
    size_t brksize;
};
