#include <gravity_compiler.h>
#include <gravity_macros.h>
#include <gravity_core.h>
#include <gravity_vm.h>
#include <gravity_vmmacros.h>

#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "options.hxx"

static const char* usages[] = {
    "exthost -f [file] [args]"
};

static void ErrorReportCallback(gravity_vm *vm, error_type_t error_type, const char *message,
                   error_desc_t error_desc, void *xdata);

int main(int argc, char** argv)
{
    const char* fName = NULL;
    CommandLineParser parser(argc, (const char**) argv);
    fName = parser.GetCommandOption("-f").c_str();

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

static void ErrorReportCallback(gravity_vm *vm, error_type_t error_type, const char *message,
                   error_desc_t error_desc, void *xdata)
{
    printf("%s\n", message);
    exit(0);
}