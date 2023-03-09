#include "Path.hpp"
#include "Core.hpp"
#include <filesystem>
#include <sstream>

using namespace RightEngine;

std::unordered_map<std::string, std::string> Path::aliasMap;
bool Path::initialized = false;

void Path::Init()
{
    R_CORE_ASSERT(!initialized, "");
    initialized = true;
    aliasMap = {
        { "/Engine", G_ENGINE_ASSET_DIR },
        { "/", G_ASSET_DIR }
    };
}

std::string Path::Absolute(const std::string& enginePath)
{
    std::string fullPath;
    for (const auto& [alias, path] : aliasMap)
    {
	    if (enginePath.rfind(alias, 0) == 0)
	    {
            fullPath.append(enginePath.substr(alias.size()));
            R_CORE_ASSERT(!fullPath.empty(), "");
            if (fullPath.at(0) != '/')
            {
                fullPath = '/' + fullPath;
            }
            fullPath = path + fullPath;
            break;
	    }
    }
    R_CORE_ASSERT(!fullPath.empty(), "");
    return fullPath;
}
