#pragma once

#include "tux_pal.h"
#include "tux_arch.h"
#include "mmap.h"
#include "boxmap.h"

struct PlatOptions {
    size_t pagesize;
    size_t vmsize;
    bool sysexternal;
};

struct Platform {
    struct BoxMap* bm;
    struct PlatOptions opts;
    SysHandlerFn syshandler;
};

struct PlatAddrSpace {
    uintptr_t base;
    size_t size;
    uintptr_t minaddr;
    uintptr_t maxaddr;

    MMAddrSpace mm;
};

struct Sys {
    uintptr_t rtcalls[256];
    uintptr_t base;
};

struct PlatContext {
    void* kstackp;
    uintptr_t tp;
    struct TuxRegs regs;
    void* ctxp;
    struct Platform* plat;
    struct Sys* sys;
};

static inline size_t
gb(size_t x)
{
    return x * 1024 * 1024 * 1024;
}

_Thread_local extern struct PlatContext* pal_myctx;
