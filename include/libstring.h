#pragma once

#include <wchar.h>

#ifdef _WIN32
#define _LS_CALLCONV __stdcall
#else
#define _LS_CALLCONV
#endif

wchar_t* _LS_CALLCONV EnwidenString(const char* lpszContent);
int _LS_CALLCONV FormatString(char* lpszOutput, const char* lpszContent, ...);