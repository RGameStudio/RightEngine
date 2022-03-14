#pragma once

#include <string>

namespace RightEngine
{
    class Path
    {
    public:
        static std::string ConvertEnginePathToOSPath(const std::string& enginePath);
    };

}
