#pragma once

#include <duktape.h>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include "apiutils.h"
#include <condition_variable>

#define PLLOAD_NO_ENTRYPOINT 0x100 /* load the VM without checking for valid entrypoint */

namespace ext {

class DLL_PUBLIC CPlugin
{
public:
    CPlugin(const CPlugin&) = default;
    explicit CPlugin(std::string sModuleName, std::string sEntryPoint);
    ~CPlugin()
    {
        this->Unload();
        this->moduleName.clear();
        this->entryPoint.clear();
    }

    virtual void Load();

    virtual void LoadEx(int flags);
    inline void DoFile();

    virtual void TriggerHook(std::string sHookName);
    
    /**
     * Like TriggerHook, but expects a param to be passed as argument to the hook func 
     **/
    virtual void TriggerHookEx(std::string sHookName, int32_t iNumArgs);
    virtual void Unload();
    void SetGlobalVarCache(std::string sVarName, bool sValue);
    void SetGlobalVar(std::string sVarName, bool sValue);

    duk_bool_t AddGlobalProc(std::string sProcName, duk_c_function lpfnCallback, duk_idx_t isArgs);

    inline duk_context* GetInnerContext()
    {
        return this->vm;
    }

    inline std::string ModuleName()
    {
        return this->moduleName;
    }

    inline std::string EntryPoint()
    {
        return this->entryPoint;
    }

    inline bool Loaded()
    {
        return this->loaded;
    }
    CPlugin& operator=(const CPlugin&);
private:
    std::map<std::string, bool> varCache;

    void StartVM();
    void FeedVMFile(std::string sfName);
protected:
    std::string moduleName;
    std::string entryPoint;
    bool loaded = false;
    duk_context* vm;
};

class DLL_PUBLIC CThreadedPlugin : public CPlugin
{
public:
    using CPlugin::CPlugin;

    CThreadedPlugin(const CThreadedPlugin&) = default;

    ~CThreadedPlugin()
    {
        CPlugin::Unload();
    }

    void Load() override;
    void LoadEx(int32_t flags) override;
    
    void Unload() override;
    void TriggerHook(std::string sHookName) override;

    /**
     * @brief Blocks until the plugin has loaded
     */
    void JoinLoad();

    CThreadedPlugin& operator=(const CThreadedPlugin&);
    bool operator==(const CThreadedPlugin& rhs)
    {
        return this->vm == rhs.vm;
    }
private:
    void StartupMask(int isFlags);
    
    mutable std::thread* vmThread = nullptr;
    std::mutex* vmMutex = nullptr;
    std::condition_variable* vmEvent;
};
} /* namespace ext */

