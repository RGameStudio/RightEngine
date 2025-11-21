#include <Engine/Editor/EntityTreePanel.hpp>
#include <Engine/Service/Imgui/ImguiService.hpp>
#include <Engine/Service/Render/RenderService.hpp>
#include <Engine/Service/EditorService.hpp>
#include <Engine/Service/World/WorldService.hpp>
#include <RHI/Pipeline.hpp>
#include <xxhash.h>

namespace engine::editor
{

void EntityTreePanel::DrawPanel()
{
	auto& ws = Instance().Service<WorldService>();
    auto& editorService = Instance().Service<EditorService>();
    auto& world = ws.CurrentWorld();

    // Add Entity button
    if (ImGui::Button("Add Entity"))
    {
        world->GetEntityManager()->CreateEntity("New Entity");
    }

    for (const auto entity : world->View())
    {
        const auto& eName = world->GetEntityManager()->GetEntityInfo(entity).m_name;
        std::string nodeId;
        if (eName.empty())
        {
            nodeId = uuids::to_string(uuids::uuid_system_generator{}());
        }
        else
        {
            nodeId = fmt::format("{:016x}", XXH64(eName.c_str(), eName.size(), 0));
        }

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick
                                    | ImGuiTreeNodeFlags_SpanFullWidth
                                    | ImGuiTreeNodeFlags_OpenOnArrow;
        
        // Check if this entity is selected
        if (editorService.SelectedEntity() == entity)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool nodeOpen = ImGui::TreeNodeEx(nodeId.c_str(), flags, "%s", eName.c_str());
        
        if (ImGui::IsItemClicked()) 
        {
            editorService.SelectedEntity(entity);
        }
        
        if (nodeOpen)
        {
            ImGui::TreePop();
        }
    }
}

} // engine::editor
