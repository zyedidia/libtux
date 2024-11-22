#include <stdlib.h>

#include "tux.h"

struct Tux*
tux_new(struct Platform* plat)
{
    struct Tux* tux = calloc(sizeof(struct Tux), 1);
    if (!tux)
        return NULL;
    tux->plat = plat;
    return tux;
}
