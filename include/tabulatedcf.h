#pragma once

#ifdef _WIN32
#   ifndef __i386__
#       define CF_CALLCONV __stdcall
#   else
#       define CF_CALLCONV __fastcall
#   endif

#   if defined(TABCF_EXPORT_SYMBOLS)
#       define CFAPI(type) __declspec(dllexport) type CF_CALLCONV
#   elif defined(TABCF_IMPORT_SYMBOLS)
#       define CFAPI(type) __declspec(dllimport) type CF_CALLCONV
#   else
#       define CFAPI(type) type CF_CALLCONV
#   endif

#   ifndef PTHREAD_FOUND
#       define _THREAD_LOCAL __declspec(thread)
#       define _STATUSC_TYPE int
#   endif
#else
#   define CFAPI(type) type
#   ifdef PTHREAD_FOUND
#       define HAS_POSIX
#       define _STATUSC_TYPE pthread_key_t*
#       define _THREAD_LOCAL
#   endif
#endif

#include <stdio.h>

_THREAD_LOCAL extern _STATUSC_TYPE __StatusCode;

enum {
    cfError_SUCESS,
    cfError_NULL_POINTER
};

CFAPI(long) cfFileSize(FILE* fpHandle);
CFAPI(int) cfReadFile(const char* fName, FILE* fpHandle, char* lpstrContent);
CFAPI(int) cfWriteToFile(FILE* fpHandle, const char* content, size_t size);

CFAPI(int) cfSetLastError(int code);
CFAPI(int) cfGetLastError();

