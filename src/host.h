#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

struct HostFile;

struct Stat;

struct HostFile* host_open(const char* filename, int flags, int mode);

ssize_t host_read(struct HostFile* file, uint8_t* buffer, size_t size);

ssize_t host_write(struct HostFile* file, uint8_t* buffer, size_t size);

ssize_t host_seek(struct HostFile* file, size_t loc, int whence);

int host_fstat(struct HostFile* file, struct Stat* stat);

ssize_t host_getdents64(struct HostFile* file, void* dirp, size_t count);

// host_readlink

int host_close(struct HostFile* file);

int host_stat(const char* path, struct Stat* stat);

struct HostFile* host_stdin(void);

struct HostFile* host_stdout(void);

struct HostFile* host_stderr(void);

ssize_t host_getrandom(void* buf, size_t size, unsigned int flags);

void* host_mmap(void* addr, size_t length, int prot, int flags, struct HostFile* file, off_t offset);

int host_munmap(void* addr, size_t length);

int host_mprotect(void* addr, size_t length, int prot);

int host_sched_yield(void);
