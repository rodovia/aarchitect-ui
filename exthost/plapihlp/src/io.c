#include <duktape.h>

#ifdef _WIN32
#include <windows.h>
#else
static char TranslateWinConstants(int mode)
{
    switch(mode)
    {
        case 0x80000000L: /* GENERIC_READ */
            return 'r';
        case 0x40000000L: /* GENERIC_WRITE */
            return 'w'
    }

    return '?';
}
#endif

duk_ret_t PlReadFile(duk_context* vm)
{
    const char* fName = duk_require_string(vm, 0);
    int mode = duk_require_int(vm, 1);

#ifdef _WIN32
    HANDLE hFile = CreateFile(  fName, mode,
                                0, NULL, 
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                NULL
                            );
    if (hFile == NULL)
    {
        duk_push_null(vm);
        return 1;
    }

    char* buf;
    DWORD nBytes = 0;
    BOOL didRead = ReadFile(hFile, buf, 4096, &nBytes, NULL);
    if (!didRead)
    {
        duk_push_null(vm);
        return 1;
    }

    CloseHandle(hFile);
    duk_push_string(vm, buf);
    return 1;
#endif
}

int PluginMain(duk_context* vm)
{
    duk_idx_t cIndx = duk_push_object(vm);
    duk_push_c_function(vm, PlReadFile, 2);
    duk_put_prop_string(vm, cIndx, "ReadFile");

    return 1;
}
