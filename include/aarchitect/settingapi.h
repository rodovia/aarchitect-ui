#ifndef _AAR_API
#include "api.h"
#endif

#include "bits/aarbase.h"

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