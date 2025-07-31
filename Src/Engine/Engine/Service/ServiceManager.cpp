#include <Engine/Service/ServiceManager.hpp>
#include <Engine/Service/Window/WindowService.hpp>
#include <EASTL/queue.h>

namespace
{

using ServiceDepsMap = eastl::vector_map<rttr::type, eastl::vector_set<rttr::type>>;

// Topological sort - https://en.wikipedia.org/wiki/Topological_sorting
eastl::vector<rttr::type> ServiceUpdateOrder(ServiceDepsMap& dependencies) {
    eastl::unordered_map<rttr::type, int> indegree;

    for (const auto& pair : dependencies) 
    {
        indegree[pair.first];
        for (const auto& dep : pair.second) 
        {
            indegree[dep]++;
        }
    }

    eastl::queue<rttr::type> q;
    for (const auto& pair : indegree) 
    {
        if (pair.second == 0) 
        {
            q.push(pair.first);
        }
    }

    eastl::vector<rttr::type> updateOrder;
    while (!q.empty()) 
    {
        const auto curr = q.front();
        q.pop();
        updateOrder.push_back(curr);
        for (const auto& dep : dependencies[curr]) 
        {
            indegree[dep]--;
            if (indegree[dep] == 0) 
            {
                q.push(dep);
            }
        }
    }

    return updateOrder;
}

} // unnamed

namespace engine
{

void ServiceManager::Update(float dt)
{
    for (auto& service : m_updateOrder)
    {
        const auto type = service->get_type();
        auto meta = type.get_metadata(registration::C_METADATA_KEY).get_value_unsafe<IService::MetaInfo>();

        if (!meta.m_ignoreRenderStopping
            && m_renderServicesDisabled
            && ((meta.m_domain & Domain::CLIENT) == Domain::CLIENT || (meta.m_domain & Domain::EDITOR) == Domain::EDITOR))
        {
            continue;
        }
        service->Update(dt);
    }
}

void ServiceManager::PostUpdate(float dt)
{
    for (auto& service : m_postUpdateOrder)
    {
        const auto type = service->get_type();
        auto meta = type.get_metadata(registration::C_METADATA_KEY).get_value_unsafe<IService::MetaInfo>();

        if (!meta.m_ignoreRenderStopping
            && m_renderServicesDisabled
            && ((meta.m_domain & Domain::CLIENT) == Domain::CLIENT || (meta.m_domain & Domain::EDITOR) == Domain::EDITOR))
        {
            continue;
        }
        service->PostUpdate(dt);
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wrange-loop-construct"
void ServiceManager::UpdateDependencyOrder()
{
    ServiceDepsMap updateDependenciesMap;
    ServiceDepsMap postUpdateDependenciesMap;

    for (const auto [type, _] : m_servicesMap)
    {
        const auto metadata = type.get_metadata(registration::C_METADATA_KEY).get_value_unsafe<IService::MetaInfo>();

        auto& typeDeps = updateDependenciesMap[type];
        typeDeps.insert(metadata.m_updateBefore.begin(), metadata.m_updateBefore.end());
        ENGINE_ASSERT(typeDeps.find(type) == typeDeps.end());

        for (const auto depType : metadata.m_updateAfter)
        {
            updateDependenciesMap[depType].insert(type);
        }
    }

    for (const auto [type, _] : m_servicesMap)
    {
        const auto metadata = type.get_metadata(registration::C_METADATA_KEY).get_value_unsafe<IService::MetaInfo>();

        auto& typeDeps = postUpdateDependenciesMap[type];
        typeDeps.insert(metadata.m_postUpdateBefore.begin(), metadata.m_postUpdateBefore.end());
        ENGINE_ASSERT(typeDeps.find(type) == typeDeps.end());

        for (const auto depType : metadata.m_postUpdateAfter)
        {
            postUpdateDependenciesMap[depType].insert(type);
        }
    }

    // TODO: Add check for loops after gathering deps maps

    const auto order = ServiceUpdateOrder(updateDependenciesMap);
    const auto postOrder = ServiceUpdateOrder(postUpdateDependenciesMap);

    // TODO: Add unit tests for checking services order

    for (const auto t : order)
    {
        m_updateOrder.emplace_back(m_services[m_servicesMap[t]]);
    }

    for (const auto t : postOrder)
    {
        m_postUpdateOrder.emplace_back(m_services[m_servicesMap[t]]);
    }
}

void ServiceManager::ToggleRenderServices(bool disable)
{
    m_renderServicesDisabled = disable;
}

#pragma clang diagnostic pop

void ServiceManager::Destroy()
{
    m_updateOrder.clear();
    m_postUpdateOrder.clear();

    for (int i = static_cast<int>(m_services.size()) - 1; i > -1; i--)
    {
        auto& service = m_services[i];
        const auto type = service->get_type();
        service.reset();
        core::log::debug("[ServiceManager] Destroyed service '{}' successfully", type.get_name());
    }
}

void ServiceManager::InitCallbacks()
{
    Instance().Service<WindowService>().SubscribeOnWindowCollapse(
        [this](bool isCollapsed)
    {
        ToggleRenderServices(isCollapsed);
    });
}
} // namespace engine