#pragma once

#include <Engine/Editor/Panel.hpp>
#include <Engine/ECS/EntityManager.hpp>
#include <eastl/vector.h>
#include <eastl/string.h>
#include <rttr/type.h>
#include <entt/entt.hpp>

namespace engine::editor
{

class ENGINE_API ComponentPanel final : public Panel
{
public:
	ComponentPanel() : Panel("Components")
	{}

private:
	virtual void DrawPanel() override;
	void DrawAddComponentDropdown(entt::entity selectedEntity);
	eastl::vector<rttr::type> GetAvailableComponentTypes(entt::entity entity);
	void AddComponentToEntity(entt::entity entity, rttr::type componentType);
};

} // engine::editor