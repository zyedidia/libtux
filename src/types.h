#pragma once

#include "arch_types.h"

struct Dirent {
    tux_ino_t d_ino;
    tux_off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};

enum {
    TUX_EPERM  = 1,
    TUX_ENOENT = 2,
    TUX_EBADF  = 9,
    TUX_EACCES = 13,
    TUX_EFAULT = 14,
    TUX_EINVAL = 22,
    TUX_EMFILE = 24,
    TUX_ENOSYS = 38,
};

enum {
    TUX_SEEK_SET  = 0,
    TUX_SEEK_CUR  = 1,
    TUX_SEEK_END  = 2,
};

enum {
    TUX_O_RDONLY    = 0x0,
    TUX_O_WRONLY    = 0x1,
    TUX_O_RDWR      = 0x2,
    TUX_O_CREAT     = 0x40,
    TUX_O_TRUNC     = 0x200,
    TUX_O_APPEND    = 0x400,
    TUX_O_NONBLOCK  = 0x800,
    TUX_O_DIRECTORY = 0x10000,
    TUX_O_CLOEXEC   = 0x80000,
};

enum {
    TUX_AT_FDCWD      = -100,
    TUX_AT_EMPTY_PATH = 0x1000,
};

enum {
    TUX_GRND_NONBLOCK = 1,
    TUX_GRND_RANDOM   = 2,
};

enum {
    TUX_FUTEX_WAIT           = 0,
    TUX_FUTEX_WAKE           = 1,
    TUX_FUTEX_WAIT_BITSET    = 9,
    TUX_FUTEX_PRIVATE_FLAG   = 128,
    TUX_FUTEX_CLOCK_REALTIME = 256,
};
