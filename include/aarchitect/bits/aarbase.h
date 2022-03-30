#pragma once

#include <stdint.h>

/* In general, this file should not be included directly.
 */

struct _USERCFG {
    struct {
        const char* host;
        int32_t port;
    } server;
    const char* username;
    uint8_t* avatar;
};

typedef struct _USERCFG aarUserConfiguration;

#define ERROR_OK        0x100
#define ERROR_NO_MEMORY 0x101
#define ERROR_FS        0x102
#define ERROR_CWRITE    0x103
#define ERROR_NULL      0x104

/* Compatibility with older code */
#define aarError_SUCCES ERROR_OK
#define aarError_NOMEMR ERROR_NO_MEMORY
#define aarError_FERROR ERROR_FS
#define aarError_CWRITE ERROR_CWRITE
#define aarError_NULLPT ERROR_NULL

enum {
    aarMessageBoxSeverity_Error,
    aarMessageBoxSeverity_Warning,
    aarMessageBoxSeverity_Ok,
    aarRequest_DESTROY_WINDOW
};