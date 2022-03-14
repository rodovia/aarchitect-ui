#include <gravity_compiler.h>
#include <gravity_macros.h>
#include <gravity_core.h>
#include <gravity_vm.h>
#include <gravity_vmmacros.h>

#include <stdio.h>
#include <stdlib.h>

#include "main.hxx"
#include "io.h"
#include "options.hxx"

static const char* usages[] = {
    "exthost -f [file] [args]"
};

#ifdef _WIN32
#   include <windows.h>
HANDLE g_EventHandle = NULL;

HANDLE GetCrashEventHandle()
{
    return g_EventHandle;
}
#else
void* GetCrashEventHandle()
{
    return NULL;
}
#endif

static void ErrorReportCallback(gravity_vm *vm, error_type_t error_type, const char *message,
                   error_desc_t error_desc, void *xdata);

static void __EntryPoint(int argc, char** argv)
{
    const char* fName = NULL;
    const char* eventName = NULL;
    const char* ipcName = NULL;
    CommandLineParser parser(argc, (const char**) argv);
    fName = parser.GetCommandOption("-f").c_str();
    bool hasIpc = parser.CommandExists("-ipc");
    
    if (hasIpc)
    {
#ifdef _WIN32
        eventName = parser.GetCommandOption("-event").c_str();
        g_EventHandle = OpenEvent(SYNCHRONIZE, FALSE, eventName);
#endif
        ipcName = parser.GetCommandOption("-ipcname").c_str();

        // SetupIPC();
    }

    size_t fSize = 0;
    const char* szSource = shReadFile(fName, &fSize);

    gravity_delegate_t delegate = { .error_callback = ErrorReportCallback };
    gravity_compiler_t* compiler = gravity_compiler_create(&delegate);
    gravity_closure_t* closure = gravity_compiler_run(compiler, szSource, fSize, 0, 0, 1);
    gravity_vm* vm = gravity_vm_new(&delegate);

    gravity_compiler_transfer(compiler, vm);
    gravity_compiler_free(compiler);

    if (gravity_vm_runmain(vm, closure))
    {
        // retrieve returned result
        gravity_value_t result = gravity_vm_result(vm);
    
        // dump result to a C string and print it to stdout
        char buffer[512];
        gravity_value_dump(vm, result, buffer, sizeof(buffer));
        printf("RESULT: %s\n", buffer);
    }

    gravity_vm_free(vm);
    gravity_core_free();
}

#ifdef _WIN32
static int __EvalExcSearch(_EXCEPTION_POINTERS* lpExcInfo)
{
    auto ih = lpExcInfo->ExceptionRecord->ExceptionCode;
    std::printf("CRITICAL: SEH %i. IGNORED\n", ih);
    if (ih != STATUS_INTEGER_OVERFLOW &&
        ih != STATUS_FLOAT_OVERFLOW)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    return EXCEPTION_CONTINUE_EXECUTION;    
}
#endif

int main(int argc, char** argv)
{
#ifdef _WIN32
    AddVectoredExceptionHandler(1, __EvalExcSearch);
#endif
    __EntryPoint(argc, argv);
}

static void ErrorReportCallback(gravity_vm *vm, error_type_t error_type, const char *message,
                   error_desc_t error_desc, void *xdata)
{
    printf("%s\n", message);
    exit(0);
}