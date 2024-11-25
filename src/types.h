#pragma once

#include "arch_types.h"

struct Dirent {
    tux_ino_t d_ino;
    tux_off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};
