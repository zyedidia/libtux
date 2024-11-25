// For getdents64
#define _GNU_SOURCE

#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>

#include <dirent.h>
#include <sys/stat.h>

#include "print.h"
#include "cwalk.h"
#include "file.h"
#include "proc.h"
#include "syscalls/syscalls.h"

static char*
fopenflags(int flags)
{
    if ((flags & TUX_O_DIRECTORY) != 0)
        flags &= ~TUX_O_DIRECTORY;
    if ((flags & TUX_O_NONBLOCK) != 0)
        flags &= ~TUX_O_NONBLOCK;
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
    WARN("invalid fopen flags: %x", flags);
    return NULL;
}

static int
openflags(int flags)
{
    return ((flags & TUX_O_RDONLY) ? O_RDONLY : 0) |
        ((flags & TUX_O_WRONLY) ? O_WRONLY : 0) |
        ((flags & TUX_O_RDWR) ? O_RDWR : 0) |
        ((flags & TUX_O_CREAT) ? O_CREAT : 0) |
        ((flags & TUX_O_APPEND) ? O_APPEND : 0) |
        ((flags & TUX_O_NONBLOCK) ? O_NONBLOCK : 0) |
        ((flags & TUX_O_DIRECTORY) ? O_DIRECTORY : 0);
}

struct FDFile*
filenew(struct Tux* tux, const char* dir, const char* path, int flags, int mode)
{
    char buffer[TUX_PATH_MAX];
    if (!cwk_path_is_absolute(path)) {
        cwk_path_join(dir, path, buffer, sizeof(buffer));
        path = buffer;
    }

    int fullflags = flags;
    if ((flags & TUX_O_CLOEXEC) != 0)
        flags &= ~TUX_O_CLOEXEC;

    char* fflags = fopenflags(flags);
    if (!fflags)
        return NULL;

    int fd = open(path, openflags(flags), mode);
    if (fd < 0)
        return NULL;
    FILE* f = fdopen(fd, fflags);
    assert(f); // must succeed

    return filefnew(f, fullflags);
}

static FILE*
filef(void* dev)
{
    return ((struct File*) dev)->file;
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
    int x = fclose(filef(dev));
    return x;
}

int
filefstatat(const char* dir, const char* path, struct Stat* stattux, int flags)
{
    char buffer[TUX_PATH_MAX];
    if (!cwk_path_is_absolute(path)) {
        cwk_path_join(dir, path, buffer, sizeof(buffer));
        path = buffer;
    }

    // TODO: handle flags correctly in fstatat
    struct stat kstat;
    int err = syserr(stat(path, &kstat));
    if (err < 0)
        return err;
    stattux->st_dev = kstat.st_dev;
    stattux->st_ino = kstat.st_ino;
    stattux->st_nlink = kstat.st_nlink;
    stattux->st_mode = kstat.st_mode;
    stattux->st_uid = kstat.st_uid;
    stattux->st_gid = kstat.st_gid;
    stattux->st_rdev = kstat.st_rdev;
    stattux->st_size = kstat.st_size;
    stattux->st_blksize = kstat.st_blksize;
    stattux->st_blocks = kstat.st_blocks;
    stattux->st_atim.sec  = kstat.st_atim.tv_sec;
    stattux->st_atim.nsec = kstat.st_atim.tv_nsec;
    stattux->st_mtim.sec  = kstat.st_mtim.tv_sec;
    stattux->st_mtim.nsec = kstat.st_mtim.tv_nsec;
    stattux->st_ctim.sec  = kstat.st_ctim.tv_sec;
    stattux->st_ctim.nsec = kstat.st_ctim.tv_nsec;
    return 0;
}

static int
filestat(void* dev, struct TuxProc* p, struct Stat* stattux)
{
    struct stat stat;
    int err = syserr(fstat(fileno(filef(dev)), &stat));
    if (err < 0)
        return err;
    stattux->st_dev = stat.st_dev;
    stattux->st_ino = stat.st_ino;
    stattux->st_nlink = stat.st_nlink;
    stattux->st_mode = stat.st_mode;
    stattux->st_uid = stat.st_uid;
    stattux->st_gid = stat.st_gid;
    stattux->st_rdev = stat.st_rdev;
    stattux->st_size = stat.st_size;
    stattux->st_blksize = stat.st_blksize;
    stattux->st_blocks = stat.st_blocks;
    stattux->st_atim.sec  = stat.st_atim.tv_sec;
    stattux->st_atim.nsec = stat.st_atim.tv_nsec;
    stattux->st_mtim.sec  = stat.st_mtim.tv_sec;
    stattux->st_mtim.nsec = stat.st_mtim.tv_nsec;
    stattux->st_ctim.sec  = stat.st_ctim.tv_sec;
    stattux->st_ctim.nsec = stat.st_ctim.tv_nsec;
    return 0;
}

static ssize_t
filegetdents(void* dev, struct TuxProc* p, void* dirp, size_t count)
{
    // TODO: not portable or secure, refactor into readdir
    return syserr(getdents64(fileno(filef(dev)), dirp, count));
}

static int
filemapfd(void* dev)
{
    return fileno(filef(dev));
}

struct FDFile*
filefnew(FILE* kfile, int flags)
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
        .mapfd = filemapfd,
    };
    return ff;
err2:
    free(f);
err1:
    return NULL;
}

// The FDFile must be a File.
void
filefree(struct FDFile* file)
{
    free((struct File*) file->dev);
    free(file);
}
