#pragma once

#include <stdio.h>
#include <stdint.h>

struct PlatAddrSpace;

struct PlatContext;

struct Platform;

struct TuxAddrSpaceInfo {
    uintptr_t minaddr;
    uintptr_t maxaddr;
};

typedef uintptr_t (*SysHandlerFn)(struct PlatContext*, uintptr_t, uintptr_t, uintptr_t,
        uintptr_t, uintptr_t, uintptr_t, uintptr_t);

struct TuxPlatformFuncs {
    struct PlatAddrSpace* (*as_new)(struct Platform*);
    struct AddrSpaceInfo (*as_info)(struct PlatAddrSpace*);
    void* (*as_mmap)(struct PlatAddrSpace*, void*, size_t, int, int, int, off_t);
    int (*as_munmap)(struct PlatAddrSpace*, void*, size_t);
    int (*as_mprotect)(struct PlatAddrSpace*, void*, size_t, int);

    struct PlatContext* (*ctx_new)(struct Platform*, void* ctxp);
    void (*ctx_resume)(struct PlatContext*, struct PlatAddrSpace*);
    void* (*ctx_data)(struct PlatContext*);

    void (*plat_syshandler)(SysHandlerFn fn);
};
