#pragma once

#include "proc.h"

struct File {
    FILE* file;
    int flags;
};

struct FDFile* filefnew(FILE* file, int flags);

struct FDFile* filenew(struct Tux* tux, const char* dir, const char* name, int flags, int mode);

int filefstatat(const char* dir, const char* path, struct Stat* stat, int flags);

void filefree(struct FDFile* file);
