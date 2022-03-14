#pragma once

#ifdef _WIN32
#   include <windows.h>
HANDLE GetCrashEventHandle();
#else
void* GetCrashEventHandle(); /* will always be null */
#endif
