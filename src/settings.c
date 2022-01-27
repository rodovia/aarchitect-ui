// This file is for the I/O interf around settings.
// For the actual settings window creation, see wSettings.cc

#include <aarchitect.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

static int aarParseSettings(aarUserConfiguration* out, const cJSON* document);

int aarGetSettings(aarUserConfiguration* configuration) {
   return aarGetSettingsEx(AARCHITECT_SETTINGS_FILE, configuration);
}

int aarGetSettingsEx(const char* filename, aarUserConfiguration* configuration) {
    aarPrettyLog("Loading configuration file %s...", filename);

}

static int aarParseSettings(aarUserConfiguration* out, const cJSON* document) {
   

    return aarError_SUCCES;
}

char* aarReadFile(FILE* handle) {
    int size;
    char* content;

    fseek(handle, 0, SEEK_END);
    size = ftell(handle);
    fseek(handle, 0, SEEK_SET);

    content = (char *) malloc(size * sizeof(char));
    if (content == NULL) {
        return "";
    }

    int elemcount = fread(content, sizeof(char), size, handle);
    if (elemcount < size) {
        aarPrettyLog("Only %d elements were read. Bug? (%d)", elemcount, size);
    }

    return content;    
}

int AAR_CALLCONV aarWriteFile(FILE* handle, const char* content) {
    if (handle == NULL || content == NULL) {
        return aarError_NULLPT;
    }

    int elem = fputs(content, handle);
    if (elem == 0) {
        return aarError_FERROR;
    }
    if (elem < strlen(content)) {
        aarPrettyLog("Only %d elements were written. Bug? (%zd)", elem, strlen(content));
    }

    return aarError_SUCCES;
}

int aarLoadJSONFile(FILE* handle, cJSON* document) {

}

int aarWriteSettings(aarUserConfiguration* inUserConf) {
    return aarWriteSettingsEx(AARCHITECT_SETTINGS_FILE, inUserConf);
}

int aarWriteSettingsEx(const char* filename, aarUserConfiguration* inUserConf) {
    
}

