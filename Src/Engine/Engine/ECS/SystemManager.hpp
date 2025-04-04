#pragma once

#include <Engine/Config.hpp>
#include <Engine/Engine.hpp>
#include <Engine/ECS/System.hpp>
#include <Core/Type.hpp>
#include <Core/Log.hpp>
#include <taskflow/taskflow.hpp>

namespace engine::ecs
{

class ENGINE_API SystemManager : public core::NonCopyable
{
public:
    SystemManager(World* world);
    ~SystemManager();

    void Update(float dt);

    template<typename T>
    void Add()
    {
        static_assert(std::is_base_of_v<ISystem, T>);
        const auto type = rttr::type::get<T>();
        ENGINE_ASSERT(type.get_constructor({rttr::type::get<World*>()}).is_valid());

        const auto metadata = type.get_metadata(registration::C_METADATA_KEY).template get_value_safe<ISystem::MetaInfo>();

        if (metadata.m_domain != Domain::ALL && (Instance().Cfg().m_domain & metadata.m_domain) != metadata.m_domain)
        {
            core::log::info("[SystemManager] Skipping '{}' registration due to incompatible domain '{}' engine domain: '{}'",
                type.get_name(),
                DomainToString(metadata.m_domain),
                DomainToString(Instance().Cfg().m_domain));
            return;
        }

        m_systems.emplace_back(std::make_unique<T>(m_world));
        m_typeToSystem[rttr::type::get<T>()] = m_systems.back().get();
    }

    void UpdateDependenciesOrder();

private:
    struct ExecutionParams
    {
        float dt;
    };

    eastl::vector<std::unique_ptr<ISystem>>         m_systems;
    eastl::unordered_map<rttr::type, ISystem*>      m_typeToSystem;
    World*                                          m_world;
    tf::Taskflow                                    m_taskflow;
    ExecutionParams                                 m_execParams;
};

} // engine::ecs