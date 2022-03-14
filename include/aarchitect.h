#pragma once

#ifdef RC_INVOKED
#include "aarchitect/aarchitect_cof.h"
#else
#include "aarchitect/api.h"
#include "aarchitect/aarchitect_connection.h"

#include <wtypes.h>
#include <wchar.h>
#include <ui.h>
#include <stdio.h>
#include <libstring.h>
#include <log.h>

#define AARCHITECT_SETTINGS_FILE "cfg.tbc"


struct _USERCFG {
    struct {
        const char* host;
        int32_t port;
    } server;
    const char* username;
    uint8_t* avatar;
};

typedef struct _USERCFG aarUserConfiguration;

enum {
    aarError_SUCCES,
    aarError_NOMEMR,
    aarError_FERROR,
    aarError_CWRITE,
    aarError_NULLPT,
};

enum {
    aarMessageBoxSeverity_Error,
    aarMessageBoxSeverity_Warning,
    aarMessageBoxSeverity_Ok,
    aarRequest_DESTROY_WINDOW
};


void AAR_CALLCONV aarMessageBox(
    const wchar_t* message, 
    const wchar_t* caption, 
    int flags
);

/**
 * @brief Parses configuration file *filename*
 * 
 * @param[in] filename the name of the file to open
 * @param[in,out] configuration Writes the configuration struct to this premalloc'd pointer
 * @return int 0 if the file was sucessfully parsed. >0 otherwise
 */
int AAR_CALLCONV aarGetSettingsEx(
    const char* filename, 
    aarUserConfiguration* configuration
);
int AAR_CALLCONV aarGetSettings(
    aarUserConfiguration* configuration
);

char* AAR_CALLCONV aarReadFile(
    FILE* handle
);
int AAR_CALLCONV aarWriteFile(
    FILE* handle, 
    const char* content
);

int AAR_CALLCONV aarWriteSettings(
    aarUserConfiguration* inUserConf
);
int AAR_CALLCONV aarWriteSettingsEx(
    const char* filename, 
    aarUserConfiguration* inUserConf
);

#define aarPrettyLog(msg, ...) log_info(msg __VA_OPT__(,) __VA_ARGS__)

#endif /* !RC_INVOKED */
