#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

struct HostFile;

struct HostFile* host_open(const char* filename, const char* mode);

size_t host_read(struct HostFile* file, uint8_t* buffer, size_t size);

size_t host_write(struct HostFile* file, uint8_t* buffer, size_t size);

ssize_t host_seek(struct HostFile* file, size_t loc, int whence);

// host_stat

// host_getdents64

// host_readlink

void host_close(struct HostFile* file);

ssize_t host_getrandom(void* buf, size_t size, unsigned int flags);
