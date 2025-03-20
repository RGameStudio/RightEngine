#include <Engine/Service/Resource/Resource.hpp>

namespace engine
{

IResource::IResource(const io::fs::path& path) : m_srcPath(path)
{
}

} // engine