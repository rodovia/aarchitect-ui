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
int shCreateProcess(
                char* szProgramName,
                char* szaArgs,
                size_t ssArgsSize, 
                int iDetached,
                struct PROCESS_* lpProc
            );

#ifdef __cplusplus
} /* extern "C" */
#endif
