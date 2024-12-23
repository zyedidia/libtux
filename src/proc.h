#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <pthread.h>

#include "tux.h"
#include "tux_pal.h"

#include "types.h"
#include "futex.h"

enum {
    TUX_PATH_MAX   = 4096,
    TUX_NOFILE     = 128,
    TUX_BRKMAXSIZE = 512ULL * 1024 * 1024,
};

struct TuxProc;

struct FDFile {
    void* dev;
    size_t refs;
    pthread_mutex_t lk_refs;

    ssize_t (*read)(void*, struct TuxProc*, uint8_t*, size_t);
    ssize_t (*write)(void*, struct TuxProc*, uint8_t*, size_t);
    ssize_t (*lseek)(void*, struct TuxProc*, off_t, int);
    int     (*close)(void*, struct TuxProc*);
    int     (*stat_)(void*, struct TuxProc*, struct Stat*);
    ssize_t (*getdents)(void*, struct TuxProc*, void*, size_t);
    int     (*chown)(void*, struct TuxProc*, tux_uid_t, tux_gid_t);
    int     (*chmod)(void*, struct TuxProc*, tux_mode_t);
    int     (*truncate)(void*, struct TuxProc*, off_t);
    int     (*sync)(void*, struct TuxProc*);

    struct HostFile* (*file)(void*);
};

struct FDTable {
    struct FDFile* files[TUX_NOFILE];
    pthread_mutex_t lk;
};

struct Dir {
    struct HostFile* file;
    struct FDFile* fd;
    pthread_mutex_t lk;
};

struct TuxProc {
    struct PlatAddrSpace* p_as;
    asptr_t brkbase;
    size_t brksize;
    pthread_mutex_t lk_as;
    pthread_mutex_t lk_brk;

    struct FDTable fdtable;
    struct Dir cwd;

    struct Futexes futexes;

    struct Tux* tux;
    struct TuxAddrSpaceInfo p_info;
};

struct TuxThread {
    struct PlatContext* p_ctx;
    asptr_t stack;

    uintptr_t ctid;
    int tid;

    struct TuxProc* proc;
};

int procmapat(struct TuxProc* p, asptr_t start, size_t size, int prot, int flags, int fd, off_t offset);

int procmapany(struct TuxProc* p, size_t size, int prot, int flags, int fd, off_t offset, asptr_t* o_mapstart);

int procunmap(struct TuxProc* p, asptr_t start, size_t size);

struct TuxThread* procnewthread(struct TuxThread* p);
