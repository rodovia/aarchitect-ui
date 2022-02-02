#include <libstring.h>
#include <stdarg.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>

wchar_t* _LS_CALLCONV EnwidenString(const char* lpszContent) {
    size_t len = sizeof(lpszContent) + 1;
    int wlen = MultiByteToWideChar(CP_ACP, 0, lpszContent, len, 0, 0);
    wchar_t* wstring = (wchar_t*) malloc(sizeof(wchar_t) * len);
    MultiByteToWideChar(CP_ACP, 0, lpszContent, len, wstring, wlen);
    return wstring;
}
#else
wchar_t* _LS_CALLCONV EnwidenString(const char* lpszContent) {
    const size_t cSize = strlen(lpszContent) + 1;
    wchar_t* wc = malloc(sizeof(wchar_t) * cSize);
    mbstowcs (wc, c, cSize);

    return wc;
}

#endif

int _LS_CALLCONV FormatString(char* lpszOutput, const char* lpszContent, ...) {
    char* snOut = malloc(strlen(lpszContent) + 1024);
    if (snOut == NULL) {
        return 1;
    }
    va_list varid;
    va_start(varid, lpszContent);
    sprintf(snOut, lpszContent, varid);
    va_end(varid);

    memmove(lpszOutput, snOut, sizeof(snOut));
    free(snOut);
}
