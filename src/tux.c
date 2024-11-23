#include <stdlib.h>

#include "tux.h"
#include "tux_pal.h"
#include "proc.h"
#include "sys.h"

#include "arch_sys.h"

struct Tux*
tux_new(struct Platform* plat, struct TuxOptions opts)
{
    struct Tux* tux = malloc(sizeof(struct Tux));
    if (!tux)
        return NULL;
    *tux = (struct Tux) {
        .plat = plat,
        .opts = opts,
    };
    pal_sys_handler(plat, &arch_syshandle);
    return tux;
}

uint64_t
tux_proc_start(struct Tux* tux, struct TuxProc* p)
{
    return pal_ctx_run(p->p_ctx, p->p_as);
}
