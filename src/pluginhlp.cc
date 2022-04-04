#include <exthost.h>
#include <aarchitect.h>
#include <aarchitect/pluginhlpapi.hxx>
#include <libconfig.h++>
#include <cassert>
#include <fmt/core.h>
#include <ui.h>

#include <algorithm>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

std::vector<ext::CThreadedPlugin> g_LoadedPlugins;

bool aarLoadPlugin(std::string szRootFolder, ext::CThreadedPlugin* lpPlugin)
{
    std::filesystem::path cfgPath = szRootFolder;
    std::filesystem::path plInfofp = "_clientplugins" / cfgPath / "Plugininfo";

    FILE* plInfofile = fopen(plInfofp.string().c_str(), "r");

    libconfig::Config cfg;
    try
    {
        cfg.read(plInfofile);
    }
    catch(const libconfig::ParseException& e)
    {
        uiMsgBoxError(GetMainWindow(), 
            "Erro ao carregar uma extensão",
            fmt::format("Ficheiro de configuração mal formado: {} @ {}:{}", 
                e.getError(), e.getFile(), e.getLine()
            ).c_str()
        );
        return false;
    }

    std::string name = cfg.lookup("Name");
    std::string entryPoint = cfg.lookup("EntryPoint");
    std::filesystem::path entryPointPath = cfgPath / entryPoint;

    ext::CThreadedPlugin pl(name, entryPointPath.string());
    pl.SetGlobalVar("CLIENT", true);
    pl.Load();
    g_LoadedPlugins.push_back(pl);

    if (lpPlugin != nullptr)
        (*lpPlugin) = pl;
    
    return true;
}

bool aarGetPlugin(std::string szName, ext::CThreadedPlugin* lpPlugin)
{
    auto itervalue = std::find_if(g_LoadedPlugins.begin(), g_LoadedPlugins.end(), 
        [szName] (ext::CThreadedPlugin pl) {
            return pl.ModuleName() == szName;
        }
    );

    if (itervalue != g_LoadedPlugins.end())
    {
        *lpPlugin = *itervalue;
        return true;
    }
    return false;
}

void aarUnloadPlugin(std::string szName)
{
    ext::CThreadedPlugin* pl = static_cast<ext::CThreadedPlugin*>(malloc(sizeof(ext::CThreadedPlugin)));
    aarGetPlugin(szName, pl);

    pl->Unload();
    g_LoadedPlugins.erase(
        std::remove(
            g_LoadedPlugins.begin(), 
            g_LoadedPlugins.end(), 
            *pl), 
        g_LoadedPlugins.end()
    );
    free(pl);
}

std::vector<ext::CThreadedPlugin>
aarGetAllPlugins()
{
    return g_LoadedPlugins;
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

        aarLoadPlugin(fDirName, nullptr);
    }
}
