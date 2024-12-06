#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>

#include <dirent.h>

#include "host.h"
#include "print.h"
#include "cwalk.h"
#include "file.h"
#include "proc.h"
#include "syscalls/syscalls.h"

static bool
validflags(int flags)
{
    // TODO: validate open flags
    return true;
}

struct FDFile*
filenew(struct Tux* tux, const char* dir, const char* path, int flags, int mode)
{
    char buffer[TUX_PATH_MAX];
    if (!cwk_path_is_absolute(path)) {
        cwk_path_join(dir, path, buffer, sizeof(buffer));
        path = buffer;
    }
    if (!validflags(flags))
        return NULL;

    int fullflags = flags;
    if ((flags & TUX_O_CLOEXEC) != 0)
        flags &= ~TUX_O_CLOEXEC;

    struct HostFile* f = host_open(path, flags, mode);
    if (!f)
        return NULL;
    return filefnew(f, fullflags);
}

static struct HostFile*
filef(void* dev)
{
    return ((struct File*) dev)->file;
}

static ssize_t
fileread(void* dev, struct TuxProc* p, uint8_t* buf, size_t buflen)
{
    (void) p;
    return host_read(filef(dev), buf, buflen);
}

static ssize_t
filewrite(void* dev, struct TuxProc* p, uint8_t* buf, size_t buflen)
{
    (void) p;
    return host_write(filef(dev), buf, buflen);
}

static ssize_t
filelseek(void* dev, struct TuxProc* p, off_t off, int whence)
{
    return host_seek(filef(dev), off, whence);
}

static int
fileclose(void* dev, struct TuxProc* p)
{
    int x = host_close(filef(dev));
    return x;
}

int
filefstatat(const char* dir, const char* path, struct Stat* stat_, int flags)
{
    char buffer[TUX_PATH_MAX];
    if (!cwk_path_is_absolute(path)) {
        cwk_path_join(dir, path, buffer, sizeof(buffer));
        path = buffer;
    }

    // TODO: fstatat flags are currently ignored

    return host_stat(path, stat_);
}

static int
filestat(void* dev, struct TuxProc* p, struct Stat* stat)
{
    int err = host_fstat(filef(dev), stat);
    if (err < 0)
        return err;
    return 0;
}

static ssize_t
filegetdents(void* dev, struct TuxProc* p, void* dirp, size_t count)
{
    return host_getdents64(filef(dev), (struct Dirent*) dirp, count);
}

static struct HostFile*
filemapfile(void* dev)
{
    return filef(dev);
}

struct FDFile*
filefnew(struct HostFile* kfile, int flags)
{
    struct File* f = malloc(sizeof(struct File));
    if (!f)
        goto err1;
    *f = (struct File) {
        .file = kfile,
        .flags = flags,
    };
    struct FDFile* ff = malloc(sizeof(struct FDFile));
    if (!ff)
        goto err2;
    *ff = (struct FDFile) {
        .dev = f,
        .refs = 0,
        .read = fileread,
        .write = filewrite,
        .lseek = filelseek,
        .close = fileclose,
        .stat_ = filestat,
        .getdents = filegetdents,
        .mapfile = filemapfile,
    };
    return ff;
err2:
    free(f);
err1:
    return NULL;
}
