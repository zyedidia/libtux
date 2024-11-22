#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t* data;
    size_t size;
} buf_t;

static inline size_t
bufread(buf_t buf, void* to, size_t count, off_t offset)
{
    if (offset + count > buf.size)
        count = buf.size - offset;
    memcpy(to, &buf.data[offset], count);
    return count;
}

buf_t bufreadfile(const char* filename);
