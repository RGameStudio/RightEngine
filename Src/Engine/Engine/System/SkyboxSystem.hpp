#pragma once

#include <Engine/ECS/System.hpp>
#include <RHI/Buffer.hpp>

namespace engine
{

struct ENGINE_API SkyboxComponent : public ecs::Component<SkyboxComponent>
{
    SkyboxComponent() = default;
    ~SkyboxComponent() = default;

    RPtr<MaterialResource>  m_skyboxMaterial;
    RPtr<EnvironmentMapResource> m_environmentMap;
};

class ENGINE_API SkyboxSystem : public ecs::System<SkyboxSystem>
{
public:
    SkyboxSystem(ecs::World* world);
    virtual ~SkyboxSystem() = default;

    virtual void Update(float dt) override;

private:
    std::shared_ptr<rhi::Buffer> m_skyboxVB;
};

} // engine