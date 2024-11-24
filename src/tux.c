#include <stdlib.h>

#include "tux.h"
#include "tux_pal.h"
#include "proc.h"
#include "sys.h"
#include "engine.h"

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

    tux->fstdin = filefnew(stdin, TUX_O_RDONLY);
    if (!tux->fstdin)
        goto err1;
    tux->fstdout = filefnew(stdout, TUX_O_WRONLY);
    if (!tux->fstdout)
        goto err2;
    tux->fstderr = filefnew(stderr, TUX_O_WRONLY);
    if (!tux->fstderr)
        goto err3;
    // Bump reference counts to represent libtux's references.
    tux->fstdin->refs++;
    tux->fstdout->refs++;
    tux->fstderr->refs++;

    pal_sys_handler(plat, &arch_syshandle);
    return tux;
err3:
    filefree(tux->fstdout);
err2:
    filefree(tux->fstdin);
err1:
    free(tux);
    return NULL;
}

uint64_t
tux_proc_start(struct Tux* tux, struct TuxProc* p)
{
    return pal_ctx_run(p->p_ctx, p->p_as);
}
