#pragma once

#include <string>
#include <unordered_map>

namespace RightEngine
{
    class Path
    {
    public:
        static void Init();
        /*
         * Gets path in engine format and returns absolute path to the resource
         */
        static std::string Absolute(const std::string& enginePath);

    private:
        static std::unordered_map<std::string, std::string> aliasMap;
        static bool initialized;
    };

}
