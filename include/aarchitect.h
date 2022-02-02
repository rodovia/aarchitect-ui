#pragma once

#include <wtypes.h>
#include <wchar.h>
#include <ui.h>
#include <stdio.h>
#include <libstring.h>

#define AARCHITECT_SETTINGS_FILE "cfg.tbc"

#ifdef _WIN32
#   ifndef __i386__
#       define AAR_CALLCONV __stdcall
#   else
#       define AAR_CALLCONV __fastcall
#   endif
#else
#define AAR_CALLCONV
#endif

struct _USERCFG {
    struct {
        const char* host;
        int32_t port;
    } server;
    const char* username;
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


void AAR_CALLCONV aarMessageBox(const wchar_t* message, const wchar_t* caption, int flags);
int AAR_CALLCONV aarGetSettingsEx(const char* filename, aarUserConfiguration* configuration);
int AAR_CALLCONV aarGetSettings(aarUserConfiguration* configuration);

char* AAR_CALLCONV aarReadFile(FILE* handle);
int AAR_CALLCONV aarWriteFile(FILE* handle, const char* content);

int AAR_CALLCONV aarWriteSettings(aarUserConfiguration* inUserConf);
int AAR_CALLCONV aarWriteSettingsEx(const char* filename, aarUserConfiguration* inUserConf);

#define aarPrettyLog(msg, ...) do {                                 \
    char* __APL_FORMAT_STRING__ = malloc(sizeof(char) * sizeof(msg) + 1024); \
    FormatString(__APL_FORMAT_STRING__, msg __VA_OPT__(,) __VA_ARGS__); \
    fprintf(stderr, "%s: %s\n", __func__, __APL_FORMAT_STRING__);       \
    free(__APL_FORMAT_STRING__);                                       \
} while(0)
