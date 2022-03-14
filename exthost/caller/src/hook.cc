#define DUK_USE_ASSERTIONS
#include <duktape.h>
#include <fmt/core.h>
#include "langprocs.hxx"

#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdio>
#include <stdexcept>
#include <mutex>
#include <thread>
#include <memory>
#include <sstream>
#include <fstream>

#include <exthost.h>
#include "langprocs.hxx"

#define PLUGIN_TRIGGERHOOK_PROCNAME "EvntProc"

namespace {
std::string Slurp(std::ifstream& in)
{
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}
}

ext::CPlugin::CPlugin(std::string sModuleName, std::string sEntryPoint)
{
    this->moduleName = sModuleName;
    this->entryPoint = sEntryPoint;
}

void ext::CPlugin::Load()
{
    if (this->loaded)
        return;

    this->loaded = true;
    this->StartVM();
    this->FeedVMFile(this->entryPoint);
}

void ext::CPlugin::StartVM()
{
    this->vm = duk_create_heap_default();
    if (this->vm == NULL)
    {
        std::printf("*** CRITICAL: Failed to create a Duktape heap\n");
    }

    RegAll(this->vm);
}

void ext::CPlugin::FeedVMFile(std::string sfName)
{   
    int error;
    std::ifstream iss;
    iss.open(this->entryPoint);
    std::string content = Slurp(iss);

    duk_push_lstring(this->vm, content.c_str(), content.length());

    error = duk_peval(this->vm);
    if (error != 0)
    {
        std::printf("*** ERROR: %s\n", duk_safe_to_string(this->vm, -1));
    }

    duk_pop(this->vm); /* ignore ret val */
}

void ext::CPlugin::TriggerHook(std::string sHookName)
{
    duk_require_stack(this->vm, 2);
    duk_push_global_object(this->vm);
    duk_get_prop_string(this->vm, -1, PLUGIN_TRIGGERHOOK_PROCNAME);
    duk_push_lstring(this->vm, sHookName.c_str(), sHookName.length());
    duk_pcall(this->vm, 1);

    duk_pop(this->vm);
}

void ext::CPlugin::Unload()
{
    if (!this->loaded)
        return;
    this->loaded = false;
    duk_pop(this->vm);
    duk_destroy_heap(this->vm);
}

void ext::CThreadedPlugin::Load()
{
    std::thread vmThread(&CPlugin::Load, this);
    std::swap(vmThread, this->vmThread);
}

void ext::CThreadedPlugin::TriggerHook(std::string sHookName)
{
    const std::lock_guard<std::mutex> mutex(this->vmMutex);
    CPlugin::TriggerHook(sHookName);
}

void ext::CThreadedPlugin::Unload()
{
    CPlugin::Unload();
}

/*
struct PLUGIN
{
    std::thread vmThread;
    WrenVM* vm;
    const char* entryPath;
    const char* moduleName;
};

void EventChecker0(struct PROCESS_* lpProc)
{
#ifdef _WIN32
    shJoinProcess(lpProc);

    DWORD exitCode;
    std::printf("\n\n\nPROCESS QUIT =>> %i\n", shGetProcessExitCode(lpProc, &exitCode));
#endif
}

namespace
{



void __StartVM(struct PLUGIN* this)
{
    WrenConfiguration config;    
    wrenInitConfiguration(&config);
    config.loadModuleFn = ResolveImports;
    config.writeFn = &__Write;

    this->vm = wrenNewVM(&config);
    
}

}

PLUGIN* shLoadPlugin(const char* szName)
{
    PLUGIN* pl = static_cast<PLUGIN*>(std::malloc(sizeof(PLUGIN)));
    std::memset(pl, 0, sizeof(PLUGIN));
    
    

    return pl;
}
*/