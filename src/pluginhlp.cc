#include <exthost.h>
#include <aarchitect/pluginhlpapi.hxx>
#include <libconfig.h++>
#include <cassert>

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

std::unordered_map<std::string, ext::CThreadedPlugin> g_LoadedPlugins;

template<typename K, typename V>
std::vector<V> RetrieveValues(std::unordered_map<K, V>& m)
{
    std::vector<V> values;
    for (typename std::unordered_map<K, V>::iterator it = m.begin(); 
        it != m.end(); ++it)
    {
        values.push_back(it->second);
    }
    return values;
}

ext::CThreadedPlugin& aarLoadPlugin(std::string szRootFolder)
{
    std::filesystem::path cfgPath = szRootFolder;
    std::filesystem::path plInfofp = cfgPath / "Plugininfo";

    libconfig::Config cfg;
    cfg.readFile(plInfofp.string());
    std::string name = cfg.lookup("Name");
    std::string entryPoint = cfg.lookup("EntryPoint");
    std::filesystem::path entryPointPath = cfgPath / entryPoint;

    ext::CThreadedPlugin* pl = new ext::CThreadedPlugin(name, entryPointPath.string());
    pl->SetGlobalVar("CLIENT", true);
    g_LoadedPlugins.emplace(name, pl);

    return g_LoadedPlugins.at(name);
}

ext::CThreadedPlugin& aarGetPlugin(std::string szName)
{
    return g_LoadedPlugins.at(szName);
}

void aarUnloadPlugin(std::string szName)
{
    ext::CThreadedPlugin& pl = aarGetPlugin(szName);
    pl.TriggerHook("Unload");
    g_LoadedPlugins.erase(szName);
}

std::vector<ext::CThreadedPlugin>
aarGetAllPlugins()
{
    std::vector<ext::CThreadedPlugin> values;
    for (std::unordered_map<std::string, ext::CThreadedPlugin>::iterator it = g_LoadedPlugins.begin();
        it != g_LoadedPlugins.end(); ++it)
    {
        values.push_back(it->second);
    }

    return values;
}

void aarLoadAllPluginsEx(std::string lpFromFolder, int32_t iFlags)
{
    for (const auto& entry : std::filesystem::directory_iterator(lpFromFolder))
    {
        const std::string fDirName = entry.path().filename().string();
        if (has_flag(iFlags, LPLFDIREX_IGNORE_UNDERSCORED_PLUGINS) &&
            fDirName.rfind('_', 0))
        {
            continue;
        }

        aarLoadPlugin(fDirName);
    }
}
