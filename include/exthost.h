#pragma once

#include <duktape.h>
#include <string>
#include <thread>
#include <mutex>
#include "apiutils.h"
#include <condition_variable>

namespace ext {

class CPlugin
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
    virtual void TriggerHook(std::string sHookName);
    
    /**
     * Like TriggerHook, but expects a param to be passed as argument to the hook func 
     **/
    virtual void TriggerHookEx(std::string sHookName, int32_t iNumArgs);
    virtual void Unload();
    void SetGlobalVar(std::string sVarName, bool sValue);

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
    void StartVM();
    void FeedVMFile(std::string sfName);
protected:
    std::string moduleName;
    std::string entryPoint;
    bool loaded = false;
    duk_context* vm;
};

class CThreadedPlugin : public CPlugin
{
public:
    using CPlugin::CPlugin;

    CThreadedPlugin(const CThreadedPlugin&) = default;

    ~CThreadedPlugin()
    {
        CPlugin::Unload();
    }

    void Load() override;
    void Unload() override;
    void TriggerHook(std::string sHookName) override;
    CThreadedPlugin& operator=(const CThreadedPlugin&);
    bool operator==(const CThreadedPlugin& rhs)
    {
        return this->vm == rhs.vm;
    }
private:
    mutable std::thread* vmThread = nullptr;
    std::mutex* vmMutex = nullptr;
};
} /* namespace ext */

