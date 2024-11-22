#pragma once

#include <stdio.h>
#include <stdint.h>

enum {
    PAL_MAP_PRIVATE   = (1 << 0),
    PAL_MAP_ANONYMOUS = (1 << 1),
    PAL_MAP_FIXED     = (1 << 2),

    PAL_PROT_NONE     = 0,
    PAL_PROT_READ     = (1 << 0),
    PAL_PROT_WRITE    = (1 << 1),
    PAL_PROT_EXEC     = (1 << 2),
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

struct PlatAddrSpace*   pal_as_new(struct Platform* plat);
struct TuxAddrSpaceInfo pal_as_info(struct PlatAddrSpace* as);
struct PlatContext*     pal_ctx_new(struct Platform* plat, void* ctxp);

asptr_t                 pal_as_mmap(struct PlatAddrSpace* as, asptr_t addr, size_t size, int prot, int flags, int fd, off_t off);
int                     pal_as_munmap(struct PlatAddrSpace* as, asptr_t addr, size_t size);
int                     pal_as_mprotect(struct PlatAddrSpace* as, asptr_t addr, size_t size, int prot);
void                    pal_as_copyfrom(struct PlatAddrSpace* as, void* dst, asuserptr_t src, size_t size);
void                    pal_as_copyto(struct PlatAddrSpace* as, asuserptr_t dst, void* src, size_t size);
void                    pal_as_free(struct PlatAddrSpace* as);
// Only used if PAL_EXTERNAL_ADDRSPACE is not defined
asuserptr_t             pal_as_p2user(struct PlatAddrSpace* as, void* p);
void*                   pal_as_user2p(struct PlatAddrSpace* as, asuserptr_t asp);

void                    pal_ctx_init(struct PlatContext* ctx, asptr_t entry, asptr_t sp);
void                    pal_ctx_resume(struct PlatContext* ctx, struct PlatAddrSpace* as);
void*                   pal_ctx_data(struct PlatContext* ctx);
void                    pal_ctx_free(struct PlatContext* ctx);

void                    pal_sys_sethandler(SysHandlerFn fn);
