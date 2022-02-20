#pragma once

#include <wchar.h>
#include <stdio.h>
#include "tabulatedcf/apidecl.h"

_THREAD_LOCAL extern _STATUSC_TYPE __StatusCode;

enum {
    cfError_SUCESS,
    cfError_NULL_POINTER,
    cfError_NOOBJ,   /* Not a object */
    cfError_NOASG,  /* Not a assign member */
    cfError_UNCLOBJ /* Uncolsed object */
};

#ifdef __cplusplus
extern "C" {
#endif

/* reminder: MANYCFAPI support at most 15 functions at once */
#define X(a) CFAPI(a);
/* file operations */
MANYCFAPI(
    long cfFileSize(
        FILE* fpHandle
    ),
    int cfReadFile(
        const char* fName, 
        FILE* fpHandle, 
        wchar_t* lpstrContent
    ),
    int cfWriteToFile(
        FILE* fpHandle, 
        const char* content, 
        size_t size
    )
);

/* EH and startup */
MANYCFAPI(
    int cfSetLastError(
        int code
    ),
    int cfGetLastError(),
    void cfStart(),
    void cfEnd()
);
#undef X

#ifdef __cplusplus
} /* extern "C" */
#endif
