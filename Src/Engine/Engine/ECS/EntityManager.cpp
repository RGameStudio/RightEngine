#include <Engine/ECS/EntityManager.hpp>
#include <Engine/System/TransformSystem.hpp>
#include <Core/Profiling.hpp>

namespace engine::ecs
{

EntityManager::EntityManager(World* world) : m_world(world)
{
}

EntityManager::~EntityManager()
{
}

void EntityManager::Update()
{
    PROFILER_CPU_ZONE;

    std::lock_guard l(m_mutex);

    // Delete entities
    for (const auto e : m_pendingDeleteEntities)
    {
        m_registry.destroy(e);
        auto& uuid = m_entities[e].m_uuid;
        m_uuidToEntity.erase(uuid);
        m_entities.erase(e);
    }

    // Create entities
    for (auto& info : m_pendingCreateEntities)
    {
        const auto e = m_registry.create();
        m_uuidToEntity[info.m_uuid] = e;
        m_entities[e] = std::move(info);

        AddComponent<TransformComponent>(info.m_uuid);
    }

    m_pendingCreateEntities.clear();
    m_pendingDeleteEntities.clear();
}

uuids::uuid EntityManager::CreateEntity(std::string_view name)
{
    std::lock_guard l(m_mutex);

    auto& info = m_pendingCreateEntities.emplace_back();
    info.m_uuid = uuids::uuid_system_generator{}();

    if (name.empty())
    {
        info.m_name = fmt::format("Entity #{}", m_entities.size());
    }
    else
    {
        info.m_name = name;
    }

    return info.m_uuid;
}

void EntityManager::RemoveEntity(const uuids::uuid& uuid)
{
    std::lock_guard l(m_mutex);

    if (const auto it = m_uuidToEntity.find(uuid); it != m_uuidToEntity.end())
    {
        m_pendingDeleteEntities.emplace_back(it->second);
        return;
    }
    ENGINE_ASSERT_WITH_MESSAGE(false, fmt::format("Entity with uuid {} was not found!", uuids::to_string(uuid)));
}

void EntityManager::RemoveEntity(entt::entity e)
{
    std::lock_guard l(m_mutex);

    if (const auto it = m_entities.find(e); it != m_entities.end())
    {
        m_pendingDeleteEntities.emplace_back(e);
        return;
    }
    ENGINE_ASSERT(false);
}

const EntityManager::EntityInfo& EntityManager::GetEntityInfo(entt::entity e)
{
    if (const auto it = m_entities.find(e); it != m_entities.end())
    {
        return it->second;
    }

    ENGINE_ASSERT(false);

    static EntityInfo info{};
    return info;
}

const EntityManager::EntityInfo& EntityManager::GetEntityInfo(const uuids::uuid& uuid)
{
    if (const auto it = m_uuidToEntity.find(uuid); it != m_uuidToEntity.end())
    {
        return GetEntityInfo(it->second);
        
    }

    ENGINE_ASSERT(false);

    static EntityInfo info{};
    return info;
}

entt::entity EntityManager::GetEntity(const uuids::uuid& uuid)
{
    if (const auto it = m_uuidToEntity.find(uuid); it != m_uuidToEntity.end())
    {
        return it->second;
    }

    return {};
}

bool EntityManager::Exists(entt::entity e)
{
    if (const auto it = m_entities.find(e); it == m_entities.end())
    {
        return false;
    }
    return true;
}

bool EntityManager::Exists(const uuids::uuid& uuid)
{
    if (const auto it = m_uuidToEntity.find(uuid); it != m_uuidToEntity.end())
    {
        return Exists(it->second);
    }
    return false;
}

void EntityManager::SetEntityName(entt::entity e, std::string_view name)
{
    if (const auto it = m_entities.find(e); it != m_entities.end())
    {
        it->second.m_name = name;
        return;
    }
    ENGINE_ASSERT_WITH_MESSAGE(false, "Entity not found");
}

void EntityManager::SetEntityName(const uuids::uuid& uuid, std::string_view name)
{
    if (const auto it = m_uuidToEntity.find(uuid); it != m_uuidToEntity.end())
    {
        SetEntityName(it->second, name);
        return;
    }
    ENGINE_ASSERT_WITH_MESSAGE(false, fmt::format("Entity with uuid {} was not found!", uuids::to_string(uuid)));
}

} // engine::ecs