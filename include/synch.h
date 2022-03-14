#pragma once

/* *** Errors *** */
#define E_NO_PIPE        0x100
#define E_BROKEN_PIPE    0x101
#define E_NO_MEM         0x102
#define E_UNSPECIF       0x103
#define E_OVERFLOW       0x104
#define E_NULL           0x105
#define E_NO_PROG        0x106

#define _MAXSIZEBUF 512

#if !defined(_WIN32) && !defined(DWORD)
typedef unsigned long DWORD;
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" 
{
#endif

struct PIPE_IPC;
struct PROCESS_;

struct PIPE_IPC* shOpenIPC(const char* szPipeName);
int shWaitUntilConnected(struct PIPE_IPC* lpIpc);
int shReadProcessComm(
                        struct PIPE_IPC* lpIpc, 
                        char* outlpData, 
                        unsigned long* outoplpDataSize
                    );
int shWriteProcessComm(
                        struct PIPE_IPC* lpIpc, 
                        const char* inlpData, 
                        size_t dataSize
                        );
void shCloseIPC(struct PIPE_IPC* lpIpc);

/* *** Multiprocessing *** */
struct PROCESS_* shAllocProc();
int shCreateProcess(
    char* szProgramName,
    char* szaArgs,
    size_t ssArgsSize, 
    int iDetached,
    struct PROCESS_* lpProc
);
void* shGetCrashEvent(
    struct PROCESS_* lpProc
);
int shGetProcessExitCode(
    struct PROCESS_* lpProc,
    DWORD* lpExitCode
);
int shJoinProcess(
    struct PROCESS_* lpProc
);

#ifdef __cplusplus
} /* extern "C" */
#endif
