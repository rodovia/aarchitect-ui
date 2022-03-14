#include <duktape.h>
#include <cstdio>
#include "langprocs.hxx"

#define IGNORERETVAL(x) ((void) (x))
#define ERRORCHECK(S, e) do { \
if ((S) != (e)) \
	std::printf("*** ERROR (%s, %s): %i != %i\n", (S), (e));\
} while(0);
#define CONSOLE_CLASS_NAME "_PrimitiveConsole"

int _ConInstances = 0;

duk_ret_t ConsoleLog(duk_context* ctx)
{
    if (duk_get_top(ctx) == 0)
    {
        return DUK_RET_TYPE_ERROR;
    }
    std::printf("%s", duk_get_string(ctx, -1));
    return 0;
}

duk_ret_t ConsoleCtor(duk_context* ctx)
{
    if (!duk_is_constructor_call(ctx)) 
    {
        return DUK_RET_TYPE_ERROR;
    }
    if (_ConInstances > 0)
    {
         return DUK_RET_REFERENCE_ERROR; /* Too many instances */
    }
    _ConInstances++;

    return 0;
}

void RegConsole(duk_context* ctx)
{
    int status;
    
    duk_push_c_function(ctx, ConsoleCtor, 0);

    duk_push_object(ctx);
        duk_push_c_function(ctx, ConsoleLog, 1);
        duk_put_prop_string(ctx, -2, "Log");
    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_global_string(ctx, CONSOLE_CLASS_NAME);

    duk_eval_string(ctx, CONSOLE_CLASS_NAME);
    duk_pnew(ctx, 0);
    IGNORERETVAL(duk_put_global_string(ctx, "console"));
}

void RegAll(duk_context* ctx)
{
    RegConsole(ctx);
}
