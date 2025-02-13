#include <Engine/Service/Filesystem/File.hpp>
#include <Engine/Service/Filesystem/VirtualFilesystemService.hpp>
#include <Engine/Engine.hpp>
#include <fstream>

namespace engine::io
{

File::File(const fs::path& path) : m_path(path)
{
}

bool File::Read(bool binary)
{
    auto& vfs = Instance().Service<VirtualFilesystemService>();

    if (!binary)
    {
        std::ifstream t(vfs.Absolute(m_path).c_str());
        std::stringstream buffer;
        buffer << t.rdbuf();
        m_data = core::Blob(buffer.str().c_str(), buffer.str().size() + 1);
    }

    std::ifstream file(vfs.Absolute(m_path).c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    eastl::vector<char> fileBuffer(size);
    if (!file.read(fileBuffer.data(), size))
    {
        return false;
    }

    m_data = core::Blob(fileBuffer.data(), fileBuffer.size());
    return true;
}

} // engine::io