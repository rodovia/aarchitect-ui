#include <duktape.h>
#include <fmt/core.h>
#include "langprocs.hxx"

#include <cstdlib>
#include <string>
#include <cstdio>
#include <mutex>
#include <filesystem>
#include <thread>
#include <sstream>
#include <fstream>

#include <exthost.h>

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

void ext::CPlugin::SetGlobalVar(std::string sVarName, bool sValue)
{
    duk_require_stack(this->vm, 2);
    duk_push_boolean(this->vm, (int) sValue);
    duk_put_global_lstring(this->vm, sVarName.c_str(), sVarName.size() + 1);
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

void ext::CPlugin::TriggerHookEx(std::string sHookName, int32_t iNumArgs)
{
    duk_require_stack(this->vm, 2);
    duk_push_global_object(this->vm);
    duk_get_prop_string(this->vm, -1, PLUGIN_TRIGGERHOOK_PROCNAME);
    duk_insert(this->vm, -(iNumArgs) + -2); /* 
                                            example:
                                            stack: (before)
                                                param
                                                func
                                                globalobject
                                                ===
                                            stack: (after)
                                                func
                                                param
                                        
                                            I hate and love stack-based APIs.
                                            */
    duk_pop(this->vm);
    duk_call(this->vm, iNumArgs + 1);
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
