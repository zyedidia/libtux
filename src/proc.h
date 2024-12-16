#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "tux.h"
#include "tux_pal.h"

#include "types.h"

enum {
    TUX_PATH_MAX   = 4096,
    TUX_NOFILE     = 128,
    TUX_BRKMAXSIZE = 512ULL * 1024 * 1024,
};

struct TuxProc;

struct FDFile {
    void* dev;
    size_t refs;

    ssize_t (*read)(void*, struct TuxProc*, uint8_t*, size_t);
    ssize_t (*write)(void*, struct TuxProc*, uint8_t*, size_t);
    ssize_t (*lseek)(void*, struct TuxProc*, off_t, int);
    int     (*close)(void*, struct TuxProc*);
    int     (*stat_)(void*, struct TuxProc*, struct Stat*);
    ssize_t (*getdents)(void*, struct TuxProc*, void*, size_t);
    int     (*chown)(void*, struct TuxProc*, tux_uid_t, tux_gid_t);
    int     (*chmod)(void*, struct TuxProc*, tux_mode_t);
    int     (*truncate)(void*, struct TuxProc*, off_t);

    struct HostFile* (*mapfile)(void*);
};

struct FDTable {
    struct FDFile* files[TUX_NOFILE];
};

struct Dir {
    char name[TUX_PATH_MAX];
    int fd;
};

struct TuxProc {
    struct PlatContext* p_ctx;
    struct PlatAddrSpace* p_as;
    struct Tux* tux;
    struct TuxAddrSpaceInfo p_info;

    struct FDTable fdtable;
    struct Dir cwd;

    asptr_t stack;

    asptr_t brkbase;
    size_t brksize;
};

int procmapat(struct TuxProc* p, asptr_t start, size_t size, int prot, int flags, int fd, off_t offset);

int procmapany(struct TuxProc* p, size_t size, int prot, int flags, int fd, off_t offset, asptr_t* o_mapstart);

int procunmap(struct TuxProc* p, asptr_t start, size_t size);
