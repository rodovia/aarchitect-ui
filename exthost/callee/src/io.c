#include "io.h"
#include <stdlib.h>
#include <stdio.h>

int64_t shGetFileSize(const char* path)
{
    struct stat* fStatus = malloc(sizeof(struct stat));
    int s = stat(path, fStatus);
    if (s < 0)
    {
        return s;
    }

    return (int64_t) fStatus->st_size;
}

const char* shReadFile(const char* path, size_t* outSize)
{
    int64_t size = shGetFileSize(path);
    FILE* hnd = fopen(path, "r");

    char* dest = malloc(sizeof(char) * size);
    memset(outSize, 0, sizeof(size_t));
    (*outSize) = fread(dest, sizeof(char), size, hnd);
    return dest;
}
