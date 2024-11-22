#pragma once

#include "mmap.h"
#include "boxmap.h"

struct PlatOptions {
    size_t pagesize;
    size_t vmsize;
};

struct Platform {
    struct BoxMap* bm;
    struct PlatOptions opts;
};

struct PlatAddrSpace {
    uintptr_t base;
    size_t size;
    uintptr_t minaddr;
    uintptr_t maxaddr;

    MMAddrSpace mm;
};

struct PlatContext {
    void* ctxp;
};
