#ifdef _WIN32
#   ifndef __i386__
#       define CF_CALLCONV __stdcall
#   else
#       define CF_CALLCONV __fastcall
#   endif

#   if defined(TABCF_EXPORT_SYMBOLS)
#       define CFAPI(type) __declspec(dllexport) CF_CALLCONV type
#   elif defined(TABCF_IMPORT_SYMBOLS)
#       define CFAPI(type) __declspec(dllimport) CF_CALLCONV type
#   else
#       define CFAPI(type) CF_CALLCONV type
#   endif
#else
#define CFAPI(type) type
#endif

CFAPI(long) cfFileSize(FILE* fpHandle);
CFAPI(int) cfReadFile(const char* fName, FILE* fpHandle, char* lpstrContent);
CFAPI(int) cfWriteToFile(FILE* fpHandle, const char* content, size_t size);

CFAPI(int) cfSetLastError(int code);
CFAPI(int) cfGetLastError();

