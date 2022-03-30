#pragma once

#ifdef RC_INVOKED
#include "aarchitect/aarchitect_cof.h"
#else
#include "aarchitect/api.h"

#define AARCHITECT_SETTINGS_FILE "cfg.tbc"

#ifdef __cplusplus
#include "aarchitect/pluginhlpapi.hxx"

extern "C" {
#endif

#include "aarchitect/aarchitect_connection.h"
#include "aarchitect/chat.h"
#include "aarchitect/settingapi.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

/* "Why not include them at the top like every normal person would do?" 
    Winsock2.h issues a "Please include Winsock2.h before windows.h" warning*/
#include <wtypes.h>
#include <wchar.h>
#include <ui.h>
#include <stdio.h>
#include <libstring.h>
#include <log.h>

#define aarPrettyLog(msg, ...) log_info(msg __VA_OPT__(,) __VA_ARGS__)

#endif /* !RC_INVOKED */
