#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int64_t shGetFileSize(const char* path);
const char* shReadFile(const char* path, size_t* outSize);

#ifdef __cplusplus
} /* extern "C" */
#endif