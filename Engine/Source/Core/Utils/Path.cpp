#include "Path.hpp"
#include <filesystem>

std::string RightEngine::Path::ConvertEnginePathToOSPath(const std::string& enginePath)
{
    const auto currentPath = std::filesystem::current_path();
    return currentPath.u8string().append(enginePath);
}
