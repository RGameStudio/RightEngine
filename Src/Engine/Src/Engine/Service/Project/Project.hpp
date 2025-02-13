#pragma once

#include <Engine/Config.hpp>
#include <Core/RTTRIntegration.hpp>
#include <filesystem>

namespace engine
{

namespace io
{
namespace fs = std::filesystem;
} // io

struct ENGINE_API ProjectData
{
    std::string                    m_name;
    uint8_t                        m_version;
    eastl::vector<rttr::variant>   m_settings;
};

class ENGINE_API Project
{
public:
    // Absolute path to a project file
    Project(const io::fs::path& path);

    template<typename T>
    T& Setting()
    {
        auto it = eastl::find_if(m_data.m_settings.begin(), m_data.m_settings.end(), [](const rttr::variant& variant)
            {
                return variant.get_type() == rttr::type::get<T>();
            });

        ENGINE_ASSERT(it != m_data.m_settings.end());

        return it->template get_value_safe<T>();
    }

private:
    const io::fs::path& m_path;
    ProjectData         m_data;
};

} // engine