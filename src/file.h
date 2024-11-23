#pragma once

#include "proc.h"

enum {
    TUX_SEEK_SET = 0,
    TUX_SEEK_CUR = 1,
    TUX_SEEK_END = 2,

    TUX_O_RDONLY = 0,
    TUX_O_WRONLY = 1,
    TUX_O_RDWR   = 2,
    TUX_O_CREAT  = 64,
    TUX_O_TRUNC  = 512,
    TUX_O_APPEND = 1024,
};

struct FDFile* filefnew(FILE* file);

struct FDFile* filenew(struct Tux* tux, const char* dir, const char* name, int flags, int mode);
