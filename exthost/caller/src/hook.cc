#include <duktape.h>
#include <duk_module_duktape.h>
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

#define IGNORED(x) ((void) (x))
#define PLUGIN_TRIGGERHOOK_PROCNAME "EvntProc"

namespace {
std::string Slurp(std::ifstream& in)
{
    std::ostringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}
}

void CrashHandler(void* uData, const char* lpMsg)
{
    IGNORED(uData);
    std::printf("*** FATAL ERROR IN DUKTAPE: %s\n", 
                (lpMsg ? lpMsg : "(unspecified)"));
    abort();
}

ext::CPlugin::CPlugin(std::string sModuleName, std::string sEntryPoint)
{
    this->moduleName = sModuleName;
    this->entryPoint = sEntryPoint;
}

ext::CPlugin& ext::CPlugin::operator=(const CPlugin& rhs)
{
    if (this->loaded)
        this->Unload();
    if (rhs.loaded)
    {
        this->vm = rhs.vm;
    }
    
    this->entryPoint = rhs.entryPoint;
    this->moduleName = rhs.moduleName;
    this->loaded = rhs.loaded;
    return *this;
}

void ext::CPlugin::Load()
{
    this->LoadEx(0);
}

void ext::CPlugin::LoadEx(int flags)
{
    if (this->loaded)
        return;

    this->StartVM();
    this->loaded = true;

    if (!(flags & PLLOAD_NO_ENTRYPOINT))
        this->FeedVMFile(this->entryPoint);
}

inline void ext::CPlugin::DoFile()
{
    this->FeedVMFile(this->entryPoint);
}

void ext::CPlugin::StartVM()
{
    this->vm = duk_create_heap( NULL,
                                NULL,
                                NULL,
                                NULL,
                                CrashHandler);
    if (this->vm == NULL)
    {
        std::printf("*** CRITICAL: Failed to create a Duktape heap\n");
        return;
    }

    for (auto const& it : this->varCache)
    {
        SetGlobalVar(it.first, it.second);
    }
    this->varCache.clear();

    duk_module_duktape_init(this->vm);
    RegAll(this->vm);
}

void ext::CPlugin::SetGlobalVar(std::string sVarName, bool sValue)
{
    duk_require_stack(this->vm, 3);
    duk_push_global_object(this->vm);
    duk_push_string(this->vm, sVarName.c_str());
    duk_push_boolean(this->vm, (int) sValue);
    duk_put_prop(this->vm, -3);

    duk_pop(this->vm); /* pop gl object */
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

void ext::CPlugin::SetGlobalVarCache(std::string sVarName, bool sValue)
{
    if (this->loaded)
    {
        this->SetGlobalVar(sVarName, sValue);
        return;
    }
        
    this->varCache.insert(std::make_pair(sVarName, sValue));
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

duk_bool_t ext::CPlugin::AddGlobalProc(std::string sProcName, 
                                       duk_c_function lpfnCallback,
                                       duk_idx_t isArgs)
{
    if (this->loaded)
        return 0;

    duk_push_global_object(this->vm);
    duk_push_c_function(this->vm, lpfnCallback, isArgs);
    return duk_put_prop_string(this->vm, -2, sProcName.c_str());
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

    this->TriggerHook("Unload");
    this->loaded = false;
    duk_pop(this->vm);
    duk_destroy_heap(this->vm);
}

void ext::CThreadedPlugin::Load()
{
    CThreadedPlugin::LoadEx(0);
}

void ext::CThreadedPlugin::LoadEx(int flags)
{
    if (this->loaded)
        return;

    this->vmThread = new std::thread(&CThreadedPlugin::StartupMask, this, flags);
    this->vmMutex = new std::mutex();
    this->vmEvent = new std::condition_variable();
}

void ext::CThreadedPlugin::StartupMask(int isFlags)
{
    CPlugin::LoadEx(isFlags);
    this->vmEvent->notify_all();
}

ext::CThreadedPlugin& ext::CThreadedPlugin::operator=(const ext::CThreadedPlugin& rhs)
{
    if (this->loaded)
        this->Unload();
    if (rhs.loaded)
    {
        std::swap(this->vmThread, rhs.vmThread);
        this->vm = rhs.vm;
    }

    this->entryPoint = rhs.entryPoint;
    this->moduleName = rhs.moduleName;
    this->loaded = rhs.loaded;
    return *this;
}

void ext::CThreadedPlugin::TriggerHook(std::string sHookName)
{
    const std::lock_guard<std::mutex> mutex(*(this->vmMutex));
    CPlugin::TriggerHook(sHookName);
}

void ext::CThreadedPlugin::JoinLoad()
{
    if (this->loaded)
        return;

    std::unique_lock<std::mutex> uLock(*(this->vmMutex));
    this->vmEvent->wait(uLock);
}

void ext::CThreadedPlugin::Unload()
{
    if (!this->loaded)
        return;
        
    delete this->vmThread;
    delete this->vmMutex;
    delete this->vmEvent;

    CPlugin::Unload();
}
