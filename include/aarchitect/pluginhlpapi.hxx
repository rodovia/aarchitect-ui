#include "../apiutils.h"
#include <string>
#include <vector>
#include <exthost.h>

#define has_flag(x, y) (x) & (y)
#define with_flag(x, y) (x) | (y)
#define add_flag(x, y) (x) |= (y)

/* *** Flags for aarLoadAllPluginsEx *** */
#define LPLFDIREX_IGNORE_UNDERSCORED_PLUGINS 0x0000102

ext::CThreadedPlugin& aarLoadPlugin(std::string szName);

void aarLoadAllPluginsEx(std::string lpFromFolder, int32_t iFlags);
#define aarLoadAllPlugins(lpFromFolder) aarLoadAllPluginsEx((lpFromFolder), 0)

void aarUnloadPlugin(std::string szName);

ext::CThreadedPlugin& aarGetPlugin(std::string szName);
std::vector<ext::CThreadedPlugin> aarGetAllPlugins();
