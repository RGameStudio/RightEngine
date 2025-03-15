#include <Engine/Service/World/WorldService.hpp>
#include <Engine/Service/Imgui/ImguiService.hpp>
#include <Engine/Service/Render/RenderService.hpp>
#include <Engine/Service/Window/WindowService.hpp>
#include <Engine/Registration.hpp>
#include <Engine/Serialization/ToJson.hpp>
#include <Engine/System/RenderSystem.hpp>
#include <Engine/System/TransformSystem.hpp>
#include <Engine/System/SkyboxSystem.hpp>
#include <Engine/Registration.hpp>
#include <Core/Profiling.hpp>

#include "Engine/Service/Filesystem/File.hpp"

RTTR_REGISTRATION
{
    engine::registration::Service<engine::WorldService>("engine::WorldService")
        .UpdateBefore<engine::ImguiService>()
        .UpdateAfter<engine::WindowService>()
        .UpdateAfter<engine::RenderService>();

    engine::registration::Class<engine::WorldEntity>("engine::WorldEntity")
        .Property("uuid", &engine::WorldEntity::m_uuid)
        .Property("name", &engine::WorldEntity::m_name)
        .Property("components", &engine::WorldEntity::m_components);

    engine::registration::Class<engine::WorldData>("engine::WorldData")
        .Property("version", &engine::WorldData::m_version)
        .Property("name", &engine::WorldData::m_name)
        .Property("entities", &engine::WorldData::m_entities);
}

namespace
{

template<typename T>
T* TryGetComponent(entt::entity e, const std::unique_ptr<engine::ecs::EntityManager>& em)
{
    const auto type = rttr::type::get<T>();
    if (const auto comp = em->TryGetComponent<T>(e); comp && engine::registration::helpers::typeRegistered(type))
    {
        return comp;
    }

    core::log::warning("Unknown component type '{}', maybe you forgot to register it?", type.get_name());
    return nullptr;
}

} // unnamed

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

void WorldService::SaveWorld()
{
    PROFILER_CPU_ZONE;

    auto data = CollectWorldData(m_world);
    auto json = engine::ToJsonString(data);

    io::File worldFile("/Worlds/test.world");
    worldFile.Write(json);
}

WorldData WorldService::CollectWorldData(const std::unique_ptr<ecs::World>& world)
{
    PROFILER_CPU_ZONE;

    if (!world)
    {
        core::log::warning("[WorldService] Trying to serialize empty world");
        return {};
    }

    WorldData data;
    data.m_name = world->Name();

    auto& em = world->GetEntityManager();

    for (const auto& [e, info] : world->GetEntityManager()->EntitiesMap())
    {
        WorldEntity worldEntity;
        worldEntity.m_name = info.m_name;
        worldEntity.m_uuid = uuids::to_string(info.m_uuid);

        if (const auto comp = TryGetComponent<TransformComponent>(e, em))
        {
            worldEntity.m_components.emplace_back(*comp);
        }

        data.m_entities.emplace_back(worldEntity);
    }

    return data;
}

} // engine