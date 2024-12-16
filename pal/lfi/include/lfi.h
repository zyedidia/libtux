#pragma once

#include <stddef.h>

struct Platform;

struct Platform* lfi_new_plat(size_t pagesize);
