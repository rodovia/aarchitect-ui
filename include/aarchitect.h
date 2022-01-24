#ifdef _WIN32
#   ifndef __i386__
#       define AAR_CALLCONV __stdcall
#   else
#       define AAR_CALLCONV __fastcall
#   endif
#endif

typedef struct {
    struct {
        const char* host;
        size_t port;
    } server;
    const char* username
} aarUserConfiguration;

AAR_CALLCONV void aarMessageBox(const wchar_t *message, const wchar_t* caption, int flags);
AAR_CALLCONV int aarGetSettingsEx(const char* filename, aarUserConfiguration* configuration);
AAR_CALLCONV int aarGetSettings(aarUserConfiguration* configuration);

AAR_CALLCONV char* aarReadFile(FILE* handle);
AAR_CALLCONV int aarWriteFile(FILE* handle, const char* content);

AAR_CALLCONV int aarWriteSettings(aarUserConfiguration* inUserConf);
AAR_CALLCONV int aarWriteSettingsEx(const char* filename, aarUserConfiguration* inUserConf);

