#include <windows.h>
#include <stdio.h>
#include <string.h>

#include <synch.h>

#define PIPE_PREFIX "\\\\.\\pipe\\"
#define PIPE_NAME "__aarchitect"

struct PIPE_IPC
{
    HANDLE pipe;
};

struct PIPE_IPC* shOpenIPC(const char* szPipeName)
{
    char neoName[sizeof(PIPE_PREFIX) + sizeof(PIPE_NAME) + sizeof(szPipeName)];
    struct PIPE_IPC* ipcPipe = malloc(sizeof(struct PIPE_IPC));
    memset(ipcPipe, 0, sizeof(struct PIPE_IPC));
    
    sprintf(neoName, "%s%s-%s", PIPE_PREFIX, PIPE_NAME, szPipeName);

    HANDLE hPipe = CreateNamedPipe(
        neoName,
        PIPE_ACCESS_DUPLEX, 
        PIPE_TYPE_MESSAGE | PIPE_REJECT_REMOTE_CLIENTS | PIPE_WAIT,
        1,
        _MAXSIZEBUF,
        _MAXSIZEBUF,
        0,    /* nDefaultTimeout */
        NULL /* lpSecurityAttributes */
    );

    ipcPipe->pipe = hPipe;
    return ipcPipe;
}

int shWaitUntilConnected(struct PIPE_IPC* lpIpc)
{
    BOOL fConnected = FALSE;
    if (lpIpc->pipe == NULL)
    {
        return E_NO_PIPE;
    }

    fConnected = ConnectNamedPipe(lpIpc->pipe, NULL)
        ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    return fConnected;
}

int shReadProcessComm(
        struct PIPE_IPC* lpIpc, 
        char* outlpData, 
        unsigned long* outoplpDataSize
    )
{
    BOOL fSuccess = FALSE;
    TCHAR* pchRequest = (TCHAR*) malloc(_MAXSIZEBUF * sizeof(TCHAR));
    DWORD cbBytesRead = 0;

    fSuccess = ReadFile(
        lpIpc->pipe,
        pchRequest,
        _MAXSIZEBUF * sizeof(TCHAR),
        &cbBytesRead,
        NULL      /* lpOverlapped */
    );

    if (!fSuccess || cbBytesRead == 0)
    {
        if (GetLastError() == ERROR_BROKEN_PIPE)
        {
            return E_BROKEN_PIPE;
        }

        return E_UNSPECIF;
    }

    if (outoplpDataSize != NULL)
    {
        (*outoplpDataSize) = cbBytesRead;
    }

    memcpy(outoplpDataSize, pchRequest, cbBytesRead);
    free(pchRequest);
    return 0;
}

int shWriteProcessComm(struct PIPE_IPC* lpIpc, const char* inlpData, size_t dataSize)
{
    if (dataSize > _MAXSIZEBUF)
    {
        return E_OVERFLOW;
    }

    if (inlpData == NULL)
    {
        return E_NULL;
    }

    BOOL fSuccess = FALSE;
    TCHAR* pchReply = (TCHAR*) malloc(dataSize * sizeof(TCHAR));
    DWORD cBytesWritten = 0;

    fSuccess = WriteFile(
        lpIpc->pipe,
        pchReply,
        dataSize,
        &cBytesWritten,
        NULL /* lpOverlapped */
    );

    free(pchReply);

    if (!fSuccess || cBytesWritten != dataSize)
    {
        return E_UNSPECIF;
    }

    return 0;
}

void shCloseIPC(struct PIPE_IPC* lpIpc)
{
    FlushFileBuffers(lpIpc->pipe);
    DisconnectNamedPipe(lpIpc->pipe);
    CloseHandle(lpIpc->pipe);
}
