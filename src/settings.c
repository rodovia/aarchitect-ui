// This file is for the I/O interf around settings.
// For the actual settings window creation, see wSettings.cc

#include <aarchitect.h>

#include <stdio.h>
#include <stdlib.h>

static int aarParseSettings(aarUserConfiguration* out, const cJSON* document);

int aarGetSettings(aarUserConfiguration* configuration) {
   return aarGetSettingsEx(AARCHITECT_SETTINGS_FILE, configuration);
}

int aarGetSettingsEx(const char* filename, aarUserConfiguration* configuration) {
    aarPrettyLog("Loading configuration file %s...", filename);

    int status;

    cJSON* document = (cJSON*) malloc(sizeof(cJSON));
    if (document == NULL) {
        return aarError_NOMEMR;
    }
    FILE* fHandle = fopen(filename, "r");
    if (fHandle == NULL) {
        return aarError_FERROR;
    }
    status = aarLoadJSONFile(fHandle, document);
    if (status != aarError_SUCCES)
        return status;
    fclose(fHandle);

    aarUserConfiguration* uconf = malloc(sizeof(uconf));
    if (uconf == NULL) {
        return aarError_NOMEMR;
    }
    status = aarParseSettings(uconf, document);
    if (status != aarError_SUCCES)
        return status;
    *configuration = *uconf;
    free(uconf);

    return aarError_SUCCES;
}

static int aarParseSettings(aarUserConfiguration* out, const cJSON* document) {
    if (document == NULL) {
        const char* errorPtr = cJSON_GetErrorPtr();
        if (errorPtr == NULL)
            errorPtr = "pointer";
        aarPrettyLog("document is NULL: %s (aarError_NULLPT returned)", errorPtr);
        return aarError_NULLPT;
    }

    if (out == NULL) {
        return aarError_NULLPT;
    }

    cJSON* username = cJSON_GetObjectItemCaseSensitive(document, "Username");
    if (cJSON_IsString(username) && (username->valuestring != NULL)) {
        out->username = username->valuestring;
    }

    cJSON* server = cJSON_GetObjectItemCaseSensitive(document, "Server");
    if (cJSON_IsObject(server) && (server->child != NULL)) {
        cJSON* serverHost = cJSON_GetObjectItemCaseSensitive(server, "Host");
        if (cJSON_IsString(serverHost) && (serverHost->valuestring != NULL)) {
            out->server.host = serverHost->valuestring;
        }

        cJSON* serverPort = cJSON_GetObjectItemCaseSensitive(server, "Port");
        if (cJSON_IsNumber(serverHost) && (serverHost->valueint != 0)) {
            out->server.port = serverHost->valuedouble;
        }
    }

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

int _CALLCONV aarWriteFile(FILE* handle, const char* content) {
    if (handle == NULL || content == NULL) {
        return aarError_NULLPT;
    }

    int elem = fputs(content, handle);
    if (elem == 0) {
        return aarError_FERROR;
    }
    if (elem < strlen(content)) {
        aarPrettyLog("Only %d elements were written. Bug? (%d)", elem, strlen(content));
    }

    return aarError_SUCCES;
}

int aarLoadJSONFile(FILE* handle, cJSON* document) {
    if (document == NULL) {
        return aarError_NULLPT;
    }

    char* content = aarReadFile(handle);
    *document = *cJSON_Parse(content);
    free(content);
    return aarError_SUCCES;
}

int aarWriteSettings(aarUserConfiguration* inUserConf) {
    return aarWriteSettingsEx(AARCHITECT_SETTINGS_FILE, inUserConf);
}

int aarWriteSettingsEx(const char* filename, aarUserConfiguration* inUserConf) {
    char* monitor = NULL;
    if (inUserConf == NULL) {
        return aarError_NULLPT;
    }
    aarPrettyLog("Attempting to write config file %s", filename);

    // Creating the JSON object
    cJSON* js = cJSON_CreateObject();
    if (cJSON_AddStringToObject(js, "Username", inUserConf->username) == NULL) {
        aarPrettyLog("Username");
        goto end;
    }

    cJSON* server = cJSON_AddObjectToObject(js, "Server");
    if (cJSON_AddStringToObject(server, "Host", inUserConf->server.host) == NULL) {
        aarPrettyLog("Host");
        goto end;
    }

    if (cJSON_AddNumberToObject(server, "Port", inUserConf->server.port) == NULL) {
        aarPrettyLog("Server");
        goto end;
    }

    monitor = cJSON_Print(js);
    if (monitor == NULL) {
        aarPrettyLog("Monitor failed to be written");
        return aarError_CWRITE;
    }

    FILE* handle = fopen(AARCHITECT_SETTINGS_FILE, "w");
    int status = aarWriteFile(handle, monitor);
    aarPrettyLog("%d %d", status, errno);
    if (status != aarError_SUCCES) {
        return status;
    }
    return aarError_SUCCES;
end:
    const char* error = cJSON_GetErrorPtr();
    if (error == NULL)
        error = "Unspecified (GetErrorPtr -> NULL)";
    aarPrettyLog("Creating JSON object failed. (%s)", error);
    return aarError_CWRITE; 
}

