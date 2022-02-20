// This file is for the I/O interf around settings.
// For the actual settings window creation, see wSettings.cc

#include <aarchitect.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <libconfig.h>

#include <stdio.h>
#include <stdlib.h>

config_t* lpConfig;

static int ParseSettings(aarUserConfiguration* out, const config_t* document);
static int LoadConfigFile(FILE* handle, config_t* document);

int AAR_CALLCONV aarGetSettings(aarUserConfiguration* configuration) {
   return aarGetSettingsEx(AARCHITECT_SETTINGS_FILE, configuration);
}

int AAR_CALLCONV aarGetSettingsEx(const char* filename, aarUserConfiguration* configuration) {
    if (lpConfig == NULL)
    {
        aarPrettyLog("OO");
        FILE* handle = fopen(filename, "r");
        config_t* cfg = calloc(1, sizeof(config_t));
        aarUserConfiguration* lpUCfg = calloc(1, sizeof(aarUserConfiguration));

        LoadConfigFile(handle, cfg);
        ParseSettings(lpUCfg, cfg);
        *configuration = *lpUCfg;
        lpConfig = cfg;
        aarPrettyLog("finished func");
        return aarError_SUCCES;
    }

    ParseSettings(configuration, lpConfig);
    return aarError_SUCCES;
}

static int ParseSettings(aarUserConfiguration* out, const config_t* document) {
    out->avatar = NULL;
    config_lookup_string(document, "server.host", &out->server.host);
    config_lookup_int(document, "server.port", &out->server.port);
    config_lookup_string(document, "username", &out->username);

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

static int LoadConfigFile(FILE* handle, config_t* document) 
{
    int status;
    config_t cfg;
    config_init(&cfg);
    config_set_options(&cfg,
                (CONFIG_OPTION_FSYNC
                    | CONFIG_OPTION_SEMICOLON_SEPARATORS
                    | CONFIG_OPTION_COLON_ASSIGNMENT_FOR_GROUPS
                    | CONFIG_OPTION_OPEN_BRACE_ON_SEPARATE_LINE));

    status = config_read(&cfg, handle);
    if (status == CONFIG_FALSE)
    {
        aarPrettyLog("ERROR while parsing file %s: %s (line %i)", 
                            config_error_file(&cfg), 
                            config_error_text(&cfg), 
                            config_error_line(&cfg));
        return aarError_FERROR;
    }

    *document = cfg;
    return aarError_SUCCES;
}

int AAR_CALLCONV aarWriteSettings(aarUserConfiguration* inUserConf) {
    return aarWriteSettingsEx(AARCHITECT_SETTINGS_FILE, inUserConf);
}

int AAR_CALLCONV aarWriteSettingsEx(const char* filename, aarUserConfiguration* inUserConf) {
    config_setting_t* serverHost;
    config_setting_t* serverPort;
    config_setting_t* username;

    config_setting_t* root = config_root_setting(lpConfig);
    serverHost = config_setting_lookup(root, "server.host");
    serverPort = config_setting_lookup(root, "server.port");
    username = config_setting_lookup(root, "username");

    config_setting_set_string(serverHost, inUserConf->server.host);
    config_setting_set_int(serverPort, inUserConf->server.port);
    config_setting_set_string(username, inUserConf->username);
    return aarError_SUCCES;
}

