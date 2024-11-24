#pragma once

#include "tux.h"
#include "file.h"

struct Tux {
    struct Platform* plat;
    struct TuxOptions opts;

    struct FDFile* fstdin;
    struct FDFile* fstdout;
    struct FDFile* fstderr;
};
