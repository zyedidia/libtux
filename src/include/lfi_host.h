#pragma once

struct HostFile;

struct HostFile* lfi_host_open(const char* filename, int flags, int mode);

int lfi_host_close(struct HostFile* file);
