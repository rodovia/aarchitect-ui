#include <exthost.h>
#include <aarchitect.h>
#include <aarchitect/pluginhlpapi.hxx>
#include <libconfig.h++>
#include <cassert>
#include <fmt/core.h>
#include <ui.h>
#include "scripterrorcode.h"

#include <algorithm>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

#ifdef _WIN32
#   include <windows.h>
#   define LOADLIBRARY LoadLibrary
#   define GETPROCADDRESS GetProcAddress
typedef HMODULE libloader_native_handle_type_t;
#else
#   include <dlfcn.h>
#   define LOADLIBRARY dlopen
#   define GETPROCADDRESS dlsym
typedef void* libloader_native_handle_type_t;
#endif

typedef int (*LibloaderEntrypoint)(duk_context*);

std::vector<ext::CThreadedPlugin> g_LoadedPlugins;
const char* g_entryPointPath;

static duk_ret_t __GetAssocFile(duk_context* vm)
{
    const char* fname = static_cast<const char*>(malloc(sizeof(char) * strlen(g_entryPointPath)));
    duk_push_string(vm, fname);
    free((void*) fname);
    return 1;
}

static duk_ret_t __LoadLibrary(duk_context* vm)
{
    int llStatus;
    LibloaderEntrypoint llEntry;
    const char* libName = duk_require_string(vm, 0);
    const char* moduleId = duk_require_string(vm, 1);

    libloader_native_handle_type_t nModule = LOADLIBRARY(libName);
    if (nModule == NULL)
    {
        duk_push_int(vm, LOADLIBRARY_NO_MODULE_LOADED);
        return 1;
    }
    llEntry = (LibloaderEntrypoint) GETPROCADDRESS(nModule, "PluginMain");
    if (llEntry == NULL)
    {
        duk_push_int(vm, LOADLIBRARY_NO_ENTRY_POINT);
        return 1;
    }
    llStatus = llEntry(vm);
    if (llStatus == 0)
    {
        duk_push_int(vm, LOADLIBRARY_NO_COOPERATE);
    }

    assert(duk_is_object(vm, -1));

    return 1;
}

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

    ext::CThreadedPlugin pl(name, "_clientplugins/std/modules/cjs_loader.js");
    pl.LoadEx(PLLOAD_NO_ENTRYPOINT);
    pl.SetGlobalVarCache("CLIENT", true);
    pl.AddGlobalProc("__GetAssocFile", __GetAssocFile, 0);
    pl.AddGlobalProc("__LoadLibrary", __LoadLibrary, 2);

    pl.DoFile();

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
