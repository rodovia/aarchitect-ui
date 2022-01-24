#include <aarchitect.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

char* aarFormatString(const char* fmt, ...) {
    char* snOut = malloc(strlen(fmt) + 128);
    if (snOut == NULL) {
        return "";
    }
    va_list varid;
    va_start(varid, fmt);

    sprintf(snOut, fmt, varid);
    va_end(varid);
    return snOut;
}
