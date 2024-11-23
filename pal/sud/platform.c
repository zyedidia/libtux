#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#include "tux_pal.h"
#include "sys.h"
#include "platform.h"

struct Platform*
sud_new_plat(void)
{
    struct PlatOptions opts = (struct PlatOptions) {
        .pagesize = getpagesize(),
        .vmsize = gb(4),
    };
    struct Platform* plat = malloc(sizeof(struct Platform));
    if (!plat)
        return NULL;

    struct BoxMap* bm = boxmap_new((struct BoxMapOptions) {
        .minalign = gb(4),
        .maxalign = gb(4),
        .guardsize = 0,
    });
    if (!bm)
        goto err1;
    if (!boxmap_reserve(bm, gb(256)))
        goto err2;

    if (!sud_init())
        goto err2;

    *plat = (struct Platform) {
        .bm = bm,
        .opts = opts,
    };
    return plat;

err2:
    boxmap_delete(bm);
err1:
    free(plat);
    return NULL;
}
