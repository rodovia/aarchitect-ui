#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <aarchitect.h>

#ifdef _WIN32
#   include <windows.h>
#endif

char* AAR_CALLCONV aarFormatString(const char* fmt, ...) {
    char* snOut = malloc(strlen(fmt) + 1024);
    if (snOut == NULL) {
        return "";
    }
    va_list varid;
    va_start(varid, fmt);

    sprintf(snOut, fmt, varid);
    va_end(varid);
    return snOut;
}

#ifdef _WIN32
const wchar_t* AAR_CALLCONV aarMultiByteTWideChar(const char* inContents) {
    size_t len = sizeof(inContents) + 1;
    int wlen = MultiByteToWideChar(CP_ACP, 0, inContents, len, 0, 0);
    wchar_t* wstring = (wchar_t*) malloc(sizeof(wchar_t) * len);
    MultiByteToWideChar(CP_ACP, 0, inContents, len, wstring, wlen);
    return wstring;
}
#endif
