#include <stdio.h>
#include <sys/mman.h>

#include "buf.h"

// TODO: make this function portable
buf_t
bufreadfile(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (!f)
        return (buf_t){NULL, 0};
    if (fseek(f, 0, SEEK_END) < 0)
        goto err;

    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    void* p = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fileno(f), 0);
    if (p == (void*) -1)
        goto err;

    fclose(f);
    return (buf_t) {
        .data = (uint8_t*) p,
        .size = size,
    };
err:
    fclose(f);
    return (buf_t){NULL, 0};
}
