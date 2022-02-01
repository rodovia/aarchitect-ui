// This file is for the I/O interf around settings.
// For the actual settings window creation, see wSettings.cc

#include <aarchitect.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#define yaml_document_t void

static int ParseSettings(aarUserConfiguration* out, const yaml_document_t* document);
static int LoadYAMLFile(FILE* handle, yaml_document_t* document);

int AAR_CALLCONV aarGetSettings(aarUserConfiguration* configuration) {
   return aarGetSettingsEx(AARCHITECT_SETTINGS_FILE, configuration);
}

int AAR_CALLCONV aarGetSettingsEx(const char* filename, aarUserConfiguration* configuration) {

    return aarError_SUCCES;
}

static int ParseSettings(aarUserConfiguration* out, const yaml_document_t* document) {
    return 0;
}

char* AAR_CALLCONV aarReadFile(FILE* handle) {
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

static int LoadYAMLFile(FILE* handle, yaml_document_t* document) {

}

int AAR_CALLCONV aarWriteSettings(aarUserConfiguration* inUserConf) {
    return aarWriteSettingsEx(AARCHITECT_SETTINGS_FILE, inUserConf);
}

int AAR_CALLCONV aarWriteSettingsEx(const char* filename, aarUserConfiguration* inUserConf) {
    
}

