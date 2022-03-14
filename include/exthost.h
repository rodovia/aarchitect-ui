#pragma once

#include <duktape.h>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace ext {

class CPlugin
{
public:
    explicit CPlugin(std::string sModuleName, std::string sEntryPoint);
    ~CPlugin()
    {
        this->Unload();
        this->moduleName.clear();
        this->entryPoint.clear();
    }

    virtual void Load();
    virtual void TriggerHook(std::string sHookName);
    virtual void Unload();
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

    ~CThreadedPlugin()
    {
        CPlugin::Unload();
    }

    void Load() override;
    void Unload() override;
    void TriggerHook(std::string sHookName) override;
private:
    mutable std::thread vmThread;
    std::mutex vmMutex;
};
}

struct PLUGIN;

struct PLUGIN* shLoadPlugin(const char* szName);
struct PLUGIN* shGetPluginFromCache(const char* szName);

int shTriggerHook(struct PLUGIN* lpPlugin, const char* szHookName, void* lpData);

