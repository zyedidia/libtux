#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "types.h"

struct HostFile;

struct Stat;

struct HostFile* host_open(const char* filename, int flags, int mode);

ssize_t host_read(struct HostFile* file, uint8_t* buffer, size_t size);

ssize_t host_write(struct HostFile* file, uint8_t* buffer, size_t size);

ssize_t host_seek(struct HostFile* file, size_t loc, int whence);

int host_fstat(struct HostFile* file, struct Stat* stat);
int host_stat(const char* path, struct Stat* stat);

int host_fchmod(struct HostFile* file, tux_mode_t mode);
int host_chmod(const char* path, tux_mode_t mode);

int host_ftruncate(struct HostFile* file, off_t length);
int host_truncate(const char* path, off_t length);

int host_fchown(struct HostFile* file, tux_uid_t owner, tux_gid_t group);
int host_chown(const char* path, tux_uid_t owner, tux_gid_t group);

ssize_t host_getdents64(struct HostFile* file, void* dirp, size_t count);

// host_readlink

int host_close(struct HostFile* file);

struct HostFile* host_stdin(void);

struct HostFile* host_stdout(void);

struct HostFile* host_stderr(void);

ssize_t host_getrandom(void* buf, size_t size, unsigned int flags);

void* host_mmap(void* addr, size_t length, int prot, int flags, struct HostFile* file, off_t offset);

int host_munmap(void* addr, size_t length);

int host_mprotect(void* addr, size_t length, int prot);

int host_sched_yield(void);

tux_time_t host_time(void);

int host_fsync(struct HostFile* file);
