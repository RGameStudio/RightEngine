#pragma once

#include <Engine/Config.hpp>
#include <Engine/Service/IService.hpp>
#include <Engine/ECS/World.hpp>

namespace engine
{

struct WorldEntity
{
    std::string m_uuid;
    std::string m_name;
    eastl::vector<rttr::variant> m_components;
};

struct WorldData
{
    int m_version = 0;
    std::string m_name;
    eastl::vector<WorldEntity> m_entities;
};

class ENGINE_API WorldService : public Service<WorldService>
{
public:
    WorldService();
    virtual ~WorldService() override;

    virtual void Update(float dt) override;
    virtual void PostUpdate(float dt) override;

    std::unique_ptr<ecs::World>& CurrentWorld() { return m_world; }

    void SaveWorld();
    bool LoadWorld(const io::fs::path& path);

    WorldData CollectWorldData(const std::unique_ptr<ecs::World>& world);
    std::unique_ptr<ecs::World> CreateWorldFromData(const WorldData& data);

private:
    std::unique_ptr<ecs::World>        m_world;
    std::unique_ptr<ecs::World>        m_newWorld;
    bool                               m_worldChanged = false;
};

} // engine