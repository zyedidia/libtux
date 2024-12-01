#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct HostFile;

void* host_mmap(void* addr, size_t length, int prot, int flags, struct HostFile* file, off_t offset);

int host_munmap(void* addr, size_t length);

int host_mprotect(void* addr, size_t length, int prot);
