#pragma once

#include <ui.h>
#include "../connection_opcodes.h"
#include "bits/aarbase.h"
#include "api.h"

void aarSendChatMessage(
    const char* content
);
int aarAboutToConnectWindowSetup(
    uiTab* group
);

void AAR_CALLCONV aarMessageBox(
    const wchar_t* message, 
    const wchar_t* caption, 
    int flags
);
