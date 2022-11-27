#include "Filesystem.hpp"

using namespace RightEngine;

std::filesystem::path Filesystem::OpenFileDialog(const std::vector<std::string>& filters)
{
   return impl->OpenFilesystemDialog(filters);
}

void Filesystem::Init()
{
    CreateImpl();
}
