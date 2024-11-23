#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>

#include "cwalk.h"
#include "file.h"
#include "proc.h"

static char*
fopenflags(int flags)
{
    switch (flags) {
    case TUX_O_RDONLY:
        return "r";
    case TUX_O_WRONLY | TUX_O_CREAT | TUX_O_TRUNC:
        return "w";
    case TUX_O_WRONLY | TUX_O_CREAT | TUX_O_APPEND:
        return "a";
    case TUX_O_RDWR:
        return "r+";
    case TUX_O_RDWR | TUX_O_CREAT | TUX_O_TRUNC:
        return "w+";
    case TUX_O_RDWR | TUX_O_CREAT | TUX_O_APPEND:
        return "a+";
    }
    fprintf(stderr, "invalid fopen flags\n");
    return NULL;
}

static int
openflags(int flags)
{
    switch (flags) {
    case TUX_O_RDONLY:
        return O_RDONLY;
    case TUX_O_WRONLY | TUX_O_CREAT | TUX_O_TRUNC:
        return O_WRONLY | O_CREAT | O_TRUNC;
    case TUX_O_WRONLY | TUX_O_CREAT | TUX_O_APPEND:
        return O_WRONLY | O_CREAT | O_APPEND;
    case TUX_O_RDWR:
        return O_RDWR;
    case TUX_O_RDWR | TUX_O_CREAT | TUX_O_TRUNC:
        return O_RDWR | O_CREAT | O_TRUNC;
    case TUX_O_RDWR | TUX_O_CREAT | TUX_O_APPEND:
        return O_RDWR | O_CREAT | O_APPEND;
    }
    fprintf(stderr, "invalid open flags\n");
    return -1;
}

struct FDFile*
filenew(struct Tux* tux, const char* dir, const char* name, int flags, int mode)
{
    char path[TUX_PATH_MAX];
    cwk_path_join(dir, name, path, sizeof(path));

    char* fflags = fopenflags(flags);
    if (!fflags)
        return NULL;

    int fd = open(path, openflags(flags), mode);
    if (fd < 0)
        return NULL;
    FILE* f = fdopen(fd, fflags);
    assert(f);

    return filefnew(f);
}

static FILE*
filef(void* dev)
{
    return (FILE*) dev;
}

static ssize_t
fileread(void* dev, struct TuxProc* p, uint8_t* buf, size_t buflen)
{
    (void) p;
    return fread(buf, 1, buflen, filef(dev));
}

static ssize_t
filewrite(void* dev, struct TuxProc* p, uint8_t* buf, size_t buflen)
{
    (void) p;
    return fwrite(buf, 1, buflen, filef(dev));
}

static ssize_t
filelseek(void* dev, struct TuxProc* p, off_t off, int whence)
{
    int fwhence;
    switch (whence) {
    case TUX_SEEK_SET:
        fwhence = SEEK_SET;
        break;
    case TUX_SEEK_CUR:
        fwhence = SEEK_CUR;
        break;
    case TUX_SEEK_END:
        fwhence = SEEK_END;
        break;
    default:
        assert(!"invalid value for whence");
    }
    return fseek(filef(dev), off, fwhence);
}

static int
fileclose(void* dev, struct TuxProc* p)
{
    return fclose(filef(dev));
}

static int
filestat(void* dev, struct TuxProc* p, struct Stat* statbuf)
{
    assert(!"unimplemented");
}

static ssize_t
filegetdents(void* dev, struct TuxProc* p, void* dirp, size_t count)
{
    assert(!"unimplemented");
}

static int
filemapfd(void* dev)
{
    assert(!"unimplemented");
}

struct FDFile*
filefnew(FILE* kfile)
{
    struct FDFile* ff = malloc(sizeof(struct FDFile));
    if (!ff)
        return NULL;
    *ff = (struct FDFile) {
        .dev = (void*) kfile,
        .refs = 0,
        .read = fileread,
        .write = filewrite,
        .lseek = filelseek,
        .close = fileclose,
        .stat = filestat,
        .getdents = filegetdents,
        .mapfd = filemapfd,
    };
    return ff;
}
