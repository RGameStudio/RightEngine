#include <Engine/Editor/MenuBarPanel.hpp>
#include <Engine/Service/World/WorldService.hpp>
#include <imgui.h>

namespace engine::editor
{

void MenuBarPanel::DrawPanel()
{
    if (ImGui::BeginMenu("General"))
    {
        bool shouldShutdownEngine = false;
        ImGui::MenuItem("Exit", nullptr, &shouldShutdownEngine);

        if (shouldShutdownEngine)
        {
            Instance().Stop();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("World"))
    {
        if (ImGui::MenuItem("Save"))
        {
            auto& ws = Instance().Service<WorldService>();
            ws.SaveWorld();
        }
        ImGui::EndMenu();
    }
}

} // engine::editor