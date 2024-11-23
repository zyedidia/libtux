#pragma once

#include "tux_pal.h"
#include "tux_arch.h"
#include "mmap.h"
#include "boxmap.h"

struct PlatOptions {
    size_t pagesize;
    size_t vmsize;
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

struct PlatContext {
    void* kstackp;
    uintptr_t ktp;
    struct TuxRegs regs;
    void* ctxp;
    struct Platform* plat;
};

static inline size_t
gb(size_t x)
{
    return x * 1024 * 1024 * 1024;
}

extern struct PlatContext* pal_myctx;

void sud_block(void);
void sud_allow(void);
