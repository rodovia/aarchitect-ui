#include <synch.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

struct PROCESS_
{
    PROCESS_INFORMATION info;
    HANDLE hCrashEvent;
};

struct PROCESS_* shAllocProc()
{
    return calloc(1, sizeof(struct PROCESS_));
}

int shCreateProcess(
    char* szProgramName,
    char* szaArgs, 
    size_t ssArgsSize, 
    int iDetached,
    struct PROCESS_* lpProc)
{
    int flags = CREATE_DEFAULT_ERROR_MODE;
    if (iDetached)
    {
        flags |= DETACHED_PROCESS;
    }

    if (szProgramName == NULL)
    {
        if (sizeof(szaArgs) == 0)
        {
            return E_NO_PROG;
        }
        szProgramName = szaArgs[0];
    }

    HANDLE hEvent = CreateEvent(
        NULL,
        TRUE,
        TRUE,
        "__ipcproc"
    );

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    WINBOOL fSucess = FALSE;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    fSucess = CreateProcessA(
        szProgramName,
        szaArgs,
        NULL, /* lpProcessAttributes */
        NULL, /* lpThreadAttributes */
        TRUE,
        flags,
        NULL, /* lpEnvironment */
        NULL, /* lpCurrentDirectory  TODO: make this a param */ 
        &si,
        &pi
    );

    printf("ERROR: ==> %i\n", GetLastError());

    if (!fSucess)
    {
        return E_UNSPECIF;
    }

    lpProc->info = pi;
    lpProc->hCrashEvent = hEvent;

    return 0;
}

HANDLE shGetCrashEvent(struct PROCESS_* lpProc)
{
    return lpProc->hCrashEvent;
}

int shGetProcessExitCode(struct PROCESS_* lpProc, DWORD* lpExitCode)
{
    return GetExitCodeProcess(lpProc->info.hProcess, lpExitCode);
}

int shJoinProcess(struct PROCESS_* lpProc)
{
    WaitForSingleObject(lpProc->info.hProcess, INFINITE);
    return 0;
}
