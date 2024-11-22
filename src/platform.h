#pragma once

#include <stdio.h>
#include <stdint.h>

enum {
    PLAT_MAP_PRIVATE   = (1 << 0),
    PLAT_MAP_ANONYMOUS = (1 << 1),
    PLAT_MAP_FIXED     = (1 << 2),

    PLAT_PROT_NONE     = 0,
    PLAT_PROT_READ     = (1 << 0),
    PLAT_PROT_WRITE    = (1 << 1),
    PLAT_PROT_EXEC     = (1 << 2),
};

struct PlatAddrSpace;

struct PlatContext;

struct Platform;

typedef uintptr_t asptr_t;
typedef uintptr_t asuserptr_t;

struct TuxAddrSpaceInfo {
    asptr_t base;
    size_t size;

    asptr_t minaddr;
    asptr_t maxaddr;
};

typedef uintptr_t (*SysHandlerFn)(struct PlatContext*, uintptr_t, uintptr_t, uintptr_t,
        uintptr_t, uintptr_t, uintptr_t, uintptr_t);

struct TuxPlatformFuncs {
    struct PlatAddrSpace*   (*as_new)(struct Platform* plat);
    struct TuxAddrSpaceInfo (*as_info)(struct PlatAddrSpace* as);
    struct PlatContext*     (*ctx_new)(struct Platform* plat, void* ctxp);

    asptr_t (*as_mmap)(struct PlatAddrSpace* as, asptr_t addr, size_t size, int prot, int flags, int fd, off_t off);
    int     (*as_munmap)(struct PlatAddrSpace* as, asptr_t addr, size_t size);
    int     (*as_mprotect)(struct PlatAddrSpace* as, asptr_t addr, size_t size, int prot);
    void    (*as_copyfrom)(struct PlatAddrSpace* as, void* dst, asuserptr_t src, size_t size);
    void    (*as_copyto)(struct PlatAddrSpace* as, asuserptr_t dst, void* src, size_t size);
    void    (*as_free)(struct PlatAddrSpace* as);

    // Only used if PLAT_EXTERNAL_ADDRSPACE is not defined
    asuserptr_t (*as_p2user)(struct PlatAddrSpace* as, void* p);
    void*       (*as_user2p)(struct PlatAddrSpace* as, asuserptr_t asp);

    void    (*ctx_init)(struct PlatContext* ctx, asptr_t entry, asptr_t sp);
    void    (*ctx_resume)(struct PlatContext* ctx, struct PlatAddrSpace* as);
    void*   (*ctx_data)(struct PlatContext* ctx);
    void    (*ctx_free)(struct PlatContext* ctx);

    void    (*plat_sethandler)(SysHandlerFn fn);
};
