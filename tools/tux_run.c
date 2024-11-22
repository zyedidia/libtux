#include <stdio.h>

#include "tux.h"
#include "sud.h"

int main() {
    struct Platform* plat = sud_new_plat();

    struct Tux* tux = tux_new(plat);

    printf("hello world\n");
    return 0;
}
