#include <Engine/Service/WorldService.hpp>
#include <Engine/Service/Imgui/ImguiService.hpp>
#include <Engine/Service/Render/RenderService.hpp>
#include <Engine/Service/Window/WindowService.hpp>
#include <Engine/Service/Project/ProjectService.hpp>
#include <Engine/Registration.hpp>
#include <Engine/System/RenderSystem.hpp>
#include <Engine/System/TransformSystem.hpp>
#include <Engine/System/SkyboxSystem.hpp>
#include <Engine/Serialization/JsonSerializer.hpp>
#include <Core/Profiling.hpp>
#include <nlohmann/json.hpp>

RTTR_REGISTRATION
{
    engine::registration::Service<engine::WorldService>("engine::WorldService")
        .UpdateBefore<engine::ImguiService>()
        .UpdateAfter<engine::WindowService>()
        .UpdateAfter<engine::RenderService>();
}

using namespace nlohmann;

namespace
{
constexpr std::string_view C_ENTITIES_KEY = "entities";
constexpr std::string_view C_NAME_KEY = "name";
constexpr std::string_view C_UUID_KEY = "uuid";
constexpr std::string_view C_COMPONENTS_KEY = "components";

template<typename T>
json SerializeComponent(entt::entity e, const std::unique_ptr<engine::ecs::EntityManager>& em)
{
    static_assert(std::is_base_of_v<engine::ecs::IComponent, T>);

    json j;
    if (const auto comp = em->TryGetComponent<T>(e))
    {
        const auto type = rttr::type::get<T>();
        j[type.get_name()] = engine::ToJsonObject(comp, type);
    }
    return j;
}

json SerializeComponents(entt::entity e, const std::unique_ptr<engine::ecs::EntityManager>& em)
{
    json j = json::array();

    j.emplace_back(std::move(SerializeComponent<engine::TransformComponent>(e, em)));

    return j;
}

}

namespace engine
{

WorldService::WorldService()
{
    m_world = std::make_unique<ecs::World>("Test world");

    m_world->GetSystemManager()->Add<TransformSystem>();
    m_world->GetSystemManager()->Add<RenderSystem>();
    m_world->GetSystemManager()->Add<CameraSystem>();
    m_world->GetSystemManager()->Add<SkyboxSystem>();

    m_world->GetSystemManager()->UpdateDependenciesOrder();
}

WorldService::~WorldService()
{
}

void WorldService::Update(float dt)
{
    PROFILER_CPU_ZONE;
    m_world->Update(dt);
}

void WorldService::PostUpdate(float dt)
{
    PROFILER_CPU_ZONE;
}

void WorldService::SaveCurrentWorld()
{
    PROFILER_CPU_ZONE;

    const auto& world = CurrentWorld();

    json worldJson;
    worldJson[C_NAME_KEY] = world->Name();
    worldJson[C_ENTITIES_KEY] = json::array();

    for (const auto& [e, info] : world->GetEntityManager()->EntitiesMap())
    {
        json entity;
        entity[C_UUID_KEY] = uuids::to_string(info.m_uuid);
        entity[C_NAME_KEY] = info.m_name;
        entity[C_COMPONENTS_KEY] = SerializeComponents(e, world->GetEntityManager());

        worldJson[C_ENTITIES_KEY].push_back(entity);
    }

    auto& vfs = Instance().Service<io::VirtualFilesystemService>();

    const auto path = vfs.Absolute("/Scenes/test.world");

    std::ofstream file(path);
    if (file.is_open())
    {
        file << worldJson.dump(4);
        file.close();
        core::log::info("[WorldService] Successfully saved world '{}'", world->Name());
    }
    else
    {
        core::log::error("[WorldService] Failed to save world '{}'", world->Name());
    }
}

} // engine