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
    std::unique_ptr<ecs::World>& LoadWorld();

    WorldData CollectWorldData(const std::unique_ptr<ecs::World>& world);

private:
    std::unique_ptr<ecs::World>        m_world;
};

} // engine