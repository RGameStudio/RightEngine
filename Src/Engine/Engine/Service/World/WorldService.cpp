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

#include "Engine/Serialization/FromJson.hpp"
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
bool TryAddComponent(entt::entity e, const std::unique_ptr<engine::ecs::EntityManager>& em, eastl::vector<rttr::variant>& comps)
{
    const auto type = rttr::type::get<T>();
    const auto typeRegistered = engine::registration::helpers::typeRegistered(type);
    if (const auto comp = em->TryGetComponent<T>(e); comp && typeRegistered)
    {
        comps.emplace_back(*comp);
        return true;
    }

    if (!typeRegistered)
    {
        core::log::warning("Unknown component type '{}', maybe you forgot to register it?", type.get_name());
    }
    return false;
}

template <typename TComponent>
void CheckAndAddComponentToEntity(const uuids::uuid& uuid,
    rttr::type type,
    const rttr::variant& variant,
    const std::unique_ptr<engine::ecs::World>& world)
{
    if (type == rttr::type::get<TComponent>())
    {
        auto& comp = world->GetEntityManager()->AddComponent<TComponent>(uuid);
        comp = variant.get_value_safe<TComponent>();
    }
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
    auto res = worldFile.Write(json);
    if (res)
    {
        core::log::info("[WorldService] World '{}' saved successfully", worldFile.Path().generic_string());
    }
}

void WorldService::LoadWorld()
{
    PROFILER_CPU_ZONE;

    io::File worldFile("/Worlds/test.world");
    auto res = worldFile.Read();
    if (!res)
    {
        core::log::error("[WorldService] World file '{}' wasnt loaded", worldFile.Path().generic_string());
    }

    // Convert raw void* to a std::string using reinterpret_cast and assuming the void* points to a char buffer
    std::string worldData(reinterpret_cast<const char*>(worldFile.Raw()), worldFile.Size());
    std::string errorBuffer;
    errorBuffer.resize(1024);

    auto json = engine::FromJsonString<WorldData>(worldData, &errorBuffer);
    if (!json.has_value())
    {
        core::log::error("[WorldService] World file '{}' want parsed", worldFile.Path().generic_string());
        return;
    }

    std::unique_ptr<ecs::World> world = std::make_unique<ecs::World>("Test world");

    for (auto& entity : json->m_entities)
    {
        auto uuidOpt = uuids::uuid::from_string(entity.m_uuid);
        if (!uuidOpt)
        {
            core::log::warning("[WorldService] Invalid uuid '{}'", entity.m_uuid);
            continue;
        }

        const auto uuid = uuidOpt.value();
        world->GetEntityManager()->CreateEntity(entity.m_name, uuid);
        world->GetEntityManager()->Update();

        for (auto& componentVariant : entity.m_components)
        {
            auto type = rttr::get_type_or_wrapped_type(componentVariant);
            if (type == rttr::type::get<TransformComponent>())
            {
                auto& comp = world->GetEntityManager()->GetComponent<TransformComponent>(uuid);
                comp = componentVariant.get_value_safe<TransformComponent>();
            }
            CheckAndAddComponentToEntity<MeshComponent>(uuid, type, componentVariant, world);
            CheckAndAddComponentToEntity<SkyboxComponent>(uuid, type, componentVariant, world);
            CheckAndAddComponentToEntity<MeshComponent>(uuid, type, componentVariant, world);
            CheckAndAddComponentToEntity<CameraComponent>(uuid, type, componentVariant, world);
        }
    }
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

        TryAddComponent<TransformComponent>(e, em, worldEntity.m_components);
        TryAddComponent<MeshComponent>(e, em, worldEntity.m_components);
        TryAddComponent<CameraComponent>(e, em, worldEntity.m_components);
        TryAddComponent<SkyboxComponent>(e, em, worldEntity.m_components);

        data.m_entities.emplace_back(worldEntity);
    }

    return data;
}

} // engine