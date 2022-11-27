#pragma once

#include <filesystem>

namespace RightEngine
{
    class FilesystemImpl
    {
    public:
        virtual std::filesystem::path OpenFilesystemDialog(const std::vector<std::string>& filters) = 0;
    };
}