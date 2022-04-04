#pragma once

#ifdef RC_INVOKED
#include "aarchitect/aarchitect_cof.h"
#else

#ifndef _AARCHITECT_H
#define _AARCHITECT_H 1 /* noc++incdirecwarn requires this macro. */

#include "aarchitect/api.h"
#define AARCHITECT_SETTINGS_FILE "cfg.tbc"

#ifdef __cplusplus
#include "aarchitect/pluginhlpapi.hxx"

extern "C" {
#endif

#include "aarchitect/aarchitect_connection.h"
#include "aarchitect/chat.h"
#include "aarchitect/settingapi.h"

/* "Why not include them at the top like every normal person would do?" 
    Winsock2.h issues a "Please include Winsock2.h before windows.h" warning*/
#include <wtypes.h>
#include <wchar.h>
#include <ui.h>
#include <stdio.h>
#include <libstring.h>
#include <log.h>

#define aarPrettyLog(msg, ...) log_info(msg __VA_OPT__(,) __VA_ARGS__)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !RC_INVOKED */
#endif
