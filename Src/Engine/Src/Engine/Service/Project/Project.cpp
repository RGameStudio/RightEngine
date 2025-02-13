#include <Engine/Service/Project/Project.hpp>
#include <Engine/Service/Filesystem/VirtualFilesystemService.hpp>
#include <Engine/Service/Filesystem/File.hpp>
#include <Engine/Registration.hpp>
#include <Engine/Serialization/ToJson.hpp>
#include <Engine/Serialization/FromJson.hpp>
#include <nlohmann/json.hpp>
#include <fstream>

RTTR_REGISTRATION
{
    using namespace engine;
    using namespace engine::registration;

    CommandLineArgs()
            .Argument(
                CommandLineArg("-prj", "--project")
                .Help("Absolute path to a project file")
                .DefaultValue("")
            );

    Class<ProjectData>("engine::ProjectData")
        .Property("name", &engine::ProjectData::m_name)
        .Property("version", &engine::ProjectData::m_version)
        .Property("settings", &engine::ProjectData::m_settings);
}

namespace engine
{

using namespace nlohmann;

Project::Project(const io::fs::path& path) : m_path(path)
{
    ENGINE_ASSERT(!path.empty());
    ENGINE_ASSERT(io::fs::exists(path));
    ENGINE_ASSERT(path.extension() == ".project");
    ENGINE_ASSERT(path.is_absolute());

    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();

    const auto string = buffer.str();

    const auto dataOpt = FromJsonString<ProjectData>(string);
    ENGINE_ASSERT_WITH_MESSAGE(dataOpt.has_value(), fmt::format("Can't read project file: '{}'", path.generic_string()).c_str());

    m_data = dataOpt.value();
}

} // engine