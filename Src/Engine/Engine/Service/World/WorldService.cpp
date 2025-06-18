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

bool TryRestoreComponent(entt::entity e, const std::unique_ptr<engine::ecs::EntityManager>& em, const rttr::variant& componentVariant)
{
    const auto type = componentVariant.get_type();
    const auto typeRegistered = engine::registration::helpers::typeRegistered(type);
    
    if (!typeRegistered)
    {
        core::log::warning("Unknown component type '{}', maybe you forgot to register it?", type.get_name());
        return false;
    }

    // Проверяем каждый известный тип компонента и добавляем соответствующий
    if (type == rttr::type::get<engine::TransformComponent>())
    {
        auto comp = componentVariant.get_value_unsafe<engine::TransformComponent>();
        em->AddComponent<engine::TransformComponent>(e, comp);
        return true;
    }
    else if (type == rttr::type::get<engine::MeshComponent>())
    {
        auto comp = componentVariant.get_value_unsafe<engine::MeshComponent>();
        em->AddComponent<engine::MeshComponent>(e, comp);
        return true;
    }
    else if (type == rttr::type::get<engine::CameraComponent>())
    {
        auto comp = componentVariant.get_value_unsafe<engine::CameraComponent>();
        em->AddComponent<engine::CameraComponent>(e, comp);
        return true;
    }
    else if (type == rttr::type::get<engine::SkyboxComponent>())
    {
        auto comp = componentVariant.get_value_unsafe<engine::SkyboxComponent>();
        em->AddComponent<engine::SkyboxComponent>(e, comp);
        return true;
    }

    core::log::warning("Unsupported component type '{}' during restoration", type.get_name());
    return false;
}

} // unnamed

namespace engine
{

WorldService::WorldService()
{
}

WorldService::~WorldService()
{
}

void WorldService::Update(float dt)
{
    PROFILER_CPU_ZONE;

    if (m_worldChanged)
    {
        m_world = std::move(m_newWorld);
        m_worldChanged = false;
        core::log::info("[WorldService] World '{}' switched successfully", m_world->Name());

        auto& rs = Instance().Service<RenderService>();
        rs.WaitAll();
    }

    if (m_world)
    {
        m_world->Update(dt);
    }
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

bool WorldService::LoadWorld(const io::fs::path& path)
{
    PROFILER_CPU_ZONE;

    io::File worldFile(path);
    auto res = worldFile.Read();
    if (!res)
    {
        core::log::error("[WorldService] Failed to read world file '{}'", worldFile.Path().generic_string());
        return false;
    }

    std::string worldData(reinterpret_cast<const char*>(worldFile.Raw()), worldFile.Size());
    std::string errorBuffer;
    errorBuffer.resize(1024);

    auto json = engine::FromJsonString<WorldData>(worldData, &errorBuffer);
    if (!json.has_value())
    {
        core::log::error("[WorldService] World file '{}' want parsed", worldFile.Path().generic_string());
        return false;
    }

    m_newWorld = CreateWorldFromData(json.value());
    m_worldChanged = true;
    core::log::info("[WorldService] World '{}' loaded successfully", m_newWorld->Name());

    return true;
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

std::unique_ptr<ecs::World> WorldService::CreateWorldFromData(const WorldData& data)
{
    PROFILER_CPU_ZONE;

    auto world = std::make_unique<ecs::World>(data.m_name);
    
    // Добавляем необходимые системы
    world->GetSystemManager()->Add<TransformSystem>();
    world->GetSystemManager()->Add<RenderSystem>();
    world->GetSystemManager()->Add<CameraSystem>();
    world->GetSystemManager()->Add<SkyboxSystem>();
    world->GetSystemManager()->UpdateDependenciesOrder();

    auto& em = world->GetEntityManager();

    // Создаем все сущности из данных
    for (const auto& worldEntity : data.m_entities)
    {
        // Парсим UUID из строки
        auto uuid = uuids::uuid::from_string(worldEntity.m_uuid);
        if (!uuid.has_value())
        {
            core::log::error("[WorldService] Invalid UUID '{}' for entity '{}'", worldEntity.m_uuid, worldEntity.m_name);
            continue;
        }

        // Создаем сущность с заданным именем
        const auto entityUuid = em->CreateEntity(worldEntity.m_name);
        em->Update(); // Обновляем чтобы сущность была создана

        const auto entity = em->GetEntity(entityUuid);
        
        // Восстанавливаем компоненты (кроме TransformComponent который добавляется автоматически)
        for (const auto& componentVariant : worldEntity.m_components)
        {
            const auto type = componentVariant.get_type();

            if (type == rttr::type::get<engine::TransformComponent>())
            {
                auto comp = componentVariant.get_value_unsafe<engine::TransformComponent>();
                auto& existingTransform = em->GetComponent<engine::TransformComponent>(entity);
                existingTransform = comp;
            }
            else
            {
                TryRestoreComponent(entity, em, componentVariant);
            }
        }
    }

    core::log::info("[WorldService] World '{}' created from data with {} entities", data.m_name, data.m_entities.size());
    return world;
}

} // engine