#include <Engine/Editor/ComponentPanel.hpp>
#include <Engine/Service/EditorService.hpp>
#include <Engine/Service/World/WorldService.hpp>
#include <Engine/System/TransformSystem.hpp>
#include <Engine/System/RenderSystem.hpp>
#include <Engine/System/SkyboxSystem.hpp>
#include <Engine/Registration.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <rttr/registration.h>

namespace
{

void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f,
	float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::TextUnformatted(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

template <class T>
void DrawComponent(entt::entity entity, std::unique_ptr<engine::ecs::EntityManager>& em, std::function<void(T&)>&& uiFunction)
{
	ENGINE_ASSERT(engine::registration::helpers::typeRegistered<T>());

	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
		| ImGuiTreeNodeFlags_Framed
		| ImGuiTreeNodeFlags_SpanAvailWidth
		| ImGuiTreeNodeFlags_AllowItemOverlap
		| ImGuiTreeNodeFlags_FramePadding;

	if (em->TryGetComponent<T>(entity))
	{
		const auto type = rttr::type::get<T>();
		const auto typeName = type.get_name().data();
		bool open = ImGui::TreeNodeEx(typeName, treeNodeFlags, "%s", typeName);

		ImGui::SameLine();
		if (ImGui::Button("+"))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(em->GetComponent<T>(entity));
			ImGui::TreePop();
		}

		if (removeComponent)
		{
			em->RemoveComponent<T>(entity);
		}
	}
}

} // unnamed

namespace engine::editor
{

void ComponentPanel::DrawPanel()
{
    auto& editorService = Instance().Service<EditorService>();
	auto& em = Instance().Service<WorldService>().CurrentWorld()->GetEntityManager();

    const auto selectedEntity = editorService.SelectedEntity();

    if (selectedEntity == C_INVALID_ENTITY)
    {
        return;
    }

    // Entity Name Panel
    auto& entityInfo = em->GetEntityInfo(selectedEntity);
    static char entityNameBuffer[256];
    strncpy(entityNameBuffer, entityInfo.m_name.c_str(), sizeof(entityNameBuffer) - 1);
    entityNameBuffer[sizeof(entityNameBuffer) - 1] = '\0';
    
    ImGui::Text("Entity Name:");
    ImGui::SameLine();
    if (ImGui::InputText("##EntityName", entityNameBuffer, sizeof(entityNameBuffer)))
    {
        em->SetEntityName(selectedEntity, std::string(entityNameBuffer));
    }
    
    ImGui::Separator();
    ImGui::Spacing();

    // Add Component Button
    DrawAddComponentDropdown(selectedEntity);
    
    ImGui::Separator();
    ImGui::Spacing();

	DrawComponent<TransformComponent>(selectedEntity, em, [](TransformComponent& t)
	{
		auto rotation = glm::degrees(glm::eulerAngles(t.m_rotation));

		DrawVec3Control("Position", t.m_position);
		DrawVec3Control("Rotation", rotation);
		DrawVec3Control("Scale", t.m_scale);

		t.m_rotation = glm::quat(glm::radians(rotation));
	});

	DrawComponent<DirectionalLightComponent>(selectedEntity, em, [](DirectionalLightComponent& l)
	{
		DrawVec3Control("Color", l.m_color);
		ImGui::Spacing();
		ImGui::DragFloat("Intensity", &l.m_intensity, 1, 0, 1000);
	});

	DrawComponent<MeshComponent>(selectedEntity, em, [](MeshComponent& l)
	{
		auto material = l.m_material->Material();

		if (auto materialBuffer = material->Buffer<PBRMaterialUB>(11))
		{
			bool dirty = false;
			ImGui::TextUnformatted("Albedo");

			if (ImGui::ColorPicker3("Color Picker", (float*)&materialBuffer->m_albedoVec))
			{
				dirty = true;
			}

			if (ImGui::Checkbox("Use albedo texture", &materialBuffer->m_useAlbedoTex))
			{
				dirty = true;
			}

			ImGui::Separator();

			ImGui::TextUnformatted("Roughness");
			if (ImGui::SliderFloat("Roughness", &materialBuffer->m_roughness, 0, 1))
			{
				dirty = true;
			}

			if (ImGui::Checkbox("Use roughness texture", &materialBuffer->m_useRoughnessTex))
			{
				dirty = true;
			}

			ImGui::Separator();

			ImGui::TextUnformatted("Metallic");
			if (ImGui::SliderFloat("Metallic", &materialBuffer->m_metallic, 0, 1))
			{
				dirty = true;
			}

			if (ImGui::Checkbox("Use metallic texture", &materialBuffer->m_useMetallicTex))
			{
				dirty = true;
			}

			if (dirty)
			{
				material->UpdateBuffer(11, *materialBuffer);
			}
		}
	});

	DrawComponent<SkyboxComponent>(selectedEntity, em, [](SkyboxComponent& s)
	{
		ImGui::TextUnformatted("Skybox Configuration");
		ImGui::TextUnformatted("Material and Environment Map paths are managed via resources");
	});

	DrawComponent<CameraComponent>(selectedEntity, em, [](CameraComponent& c)
	{
		ImGui::DragFloat("Near", &c.m_near, 0.01f, 0.001f, 100.0f);
		ImGui::DragFloat("Far", &c.m_far, 1.0f, 1.0f, 10000.0f);
		ImGui::DragFloat("Aspect Ratio", &c.m_aspectRatio, 0.01f, 0.1f, 10.0f);
		ImGui::DragFloat("FOV (radians)", &c.m_fov, 0.01f, 0.1f, 3.14f);
		
		const char* cameraTypes[] = { "Editor", "Game" };
		int currentType = static_cast<int>(c.m_type);
		if (ImGui::Combo("Type", &currentType, cameraTypes, 2))
		{
			c.m_type = static_cast<CameraComponent::Type>(currentType);
		}
		
		ImGui::Checkbox("Active", &c.m_active);
	});
}

void ComponentPanel::DrawAddComponentDropdown(entt::entity selectedEntity)
{
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }
    
    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        ImGui::TextUnformatted("Select Component Type:");
        ImGui::Separator();
        
        auto availableTypes = GetAvailableComponentTypes(selectedEntity);
        
        for (const auto& type : availableTypes)
        {
            const auto typeName = type.get_name().data();
            if (ImGui::MenuItem(typeName))
            {
                AddComponentToEntity(selectedEntity, type);
                ImGui::CloseCurrentPopup();
            }
        }
        
        if (availableTypes.empty())
        {
            ImGui::TextUnformatted("No components available to add");
        }
        
        ImGui::EndPopup();
    }
}

eastl::vector<rttr::type> ComponentPanel::GetAvailableComponentTypes(entt::entity entity)
{
    auto& em = Instance().Service<WorldService>().CurrentWorld()->GetEntityManager();
    eastl::vector<rttr::type> availableTypes;
    
    // Список всех зарегистрированных компонентов
    eastl::vector<rttr::type> allComponentTypes = {
        rttr::type::get<TransformComponent>(),
        rttr::type::get<DirectionalLightComponent>(),
        rttr::type::get<MeshComponent>(),
        rttr::type::get<CameraComponent>(),
        rttr::type::get<SkyboxComponent>()
    };
    
    for (const auto& type : allComponentTypes)
    {
        // Проверяем что компонент зарегистрирован в RTTR
        if (!engine::registration::helpers::typeRegistered(type))
            continue;
            
        // Проверяем что у энтити еще нет этого компонента
        bool hasComponent = false;
        
        if (type == rttr::type::get<TransformComponent>())
            hasComponent = em->TryGetComponent<TransformComponent>(entity) != nullptr;
        else if (type == rttr::type::get<DirectionalLightComponent>())
            hasComponent = em->TryGetComponent<DirectionalLightComponent>(entity) != nullptr;
        else if (type == rttr::type::get<MeshComponent>())
            hasComponent = em->TryGetComponent<MeshComponent>(entity) != nullptr;
        else if (type == rttr::type::get<CameraComponent>())
            hasComponent = em->TryGetComponent<CameraComponent>(entity) != nullptr;
        else if (type == rttr::type::get<SkyboxComponent>())
            hasComponent = em->TryGetComponent<SkyboxComponent>(entity) != nullptr;
            
        if (!hasComponent)
        {
            availableTypes.push_back(type);
        }
    }
    
    return availableTypes;
}

void ComponentPanel::AddComponentToEntity(entt::entity entity, rttr::type componentType)
{
    auto& em = Instance().Service<WorldService>().CurrentWorld()->GetEntityManager();
    
    if (componentType == rttr::type::get<TransformComponent>())
    {
        em->AddComponent<TransformComponent>(entity);
    }
    else if (componentType == rttr::type::get<DirectionalLightComponent>())
    {
        em->AddComponent<DirectionalLightComponent>(entity);
    }
    else if (componentType == rttr::type::get<MeshComponent>())
    {
        em->AddComponent<MeshComponent>(entity);
    }
    else if (componentType == rttr::type::get<CameraComponent>())
    {
        em->AddComponent<CameraComponent>(entity);
    }
    else if (componentType == rttr::type::get<SkyboxComponent>())
    {
        em->AddComponent<SkyboxComponent>(entity);
    }
}

} // engine::editor
