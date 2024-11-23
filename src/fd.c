#include <stdio.h>
#include <stdlib.h>

#include "fd.h"
#include "file.h"

void
fdassign(struct FDTable* t, int fd, struct FDFile* ff)
{
    ff->refs++;
    t->files[fd] = ff;
}

int
fdalloc(struct FDTable* t)
{
    int i;
    for (i = 0; i < TUX_NOFILE; i++) {
        if (t->files[i] == NULL)
            break;
    }
    if (i >= TUX_NOFILE)
        return -1;
    return i;
}

struct FDFile*
fdget(struct FDTable* t, int fd)
{
    if (fdhas(t, fd))
        return t->files[fd];
    return NULL;
}

void
fdrelease(struct FDFile* f, struct TuxProc* p)
{
    f->refs--;
    if (f->refs == 0) {
        if (f->close)
            f->close(f->dev, p);
        free(f);
    }
}

bool
fdremove(struct FDTable* t, int fd, struct TuxProc* p)
{
    if (fdhas(t, fd)) {
        fdrelease(t->files[fd], p);
        t->files[fd] = NULL;
        return true;
    }
    return false;
}

bool
fdhas(struct FDTable* t, int fd)
{
    return fd >= 0 && fd < TUX_NOFILE && t->files[fd] != NULL;
}

void
fdclear(struct FDTable* t, struct TuxProc* p)
{
    for (int fd = 0; fd < TUX_NOFILE; fd++) {
        fdremove(t, fd, p);
    }
}

void
fdinit(struct FDTable* t)
{
    fdassign(t, 0, filefnew(stdin));
    fdassign(t, 1, filefnew(stdout));
    fdassign(t, 2, filefnew(stderr));
}
