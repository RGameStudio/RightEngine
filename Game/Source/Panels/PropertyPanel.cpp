#include "PropertyPanel.hpp"
#include "Components.hpp"
#include "String.hpp"
#include "Path.hpp"
#include "AssetManager.hpp"
#include "ImGuiLayer.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <imfilebrowser.h>
#include <filesystem>

using namespace RightEngine;

namespace
{
    void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
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

    ImGui::FileBrowser fileDialog;
}

PropertyPanel::PropertyPanel(const std::shared_ptr<Scene>& aScene)
{
    scene = aScene;
}

void PropertyPanel::SetScene(const std::shared_ptr<Scene>& aScene)
{
    scene = aScene;
}

void PropertyPanel::SetSelectedEntity(const std::shared_ptr<Entity>& entity)
{
    selectedEntity = entity;
}

template<class T>
void DrawComponent(const std::string& componentName, const std::shared_ptr<Entity>& entity,
                   std::function<void(T&)> uiFunction)
{
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
                                             | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap
                                             | ImGuiTreeNodeFlags_FramePadding;
    if (entity->HasComponent<T>())
    {
        auto& component = entity->GetComponent<T>();
        bool open = ImGui::TreeNodeEx((void*) typeid(T).hash_code(), treeNodeFlags, componentName.c_str());

        if (open)
        {
            uiFunction(component);
            ImGui::TreePop();
        }
    }
}

void PropertyPanel::OnImGuiRender()
{
    ImGui::Begin("Properties");
    if (selectedEntity)
    {
        DrawComponent<TagComponent>("Tag", selectedEntity, [](auto& component)
        {
            const size_t bufSize = 256;
            char buf[bufSize];
            R_CORE_ASSERT(component.name.size() < bufSize, "");
            snprintf(buf, bufSize, component.name.c_str());
            ImGui::InputText("Entity Name", buf, bufSize);
            //TODO: Change name only if buf was changed
            component.name = std::string(buf);
            ImGui::Separator();
            ImGui::LabelText("Entity ID", "%d", component.id);
            ImGui::Separator();
            ImGui::LabelText("GUID", "%s", component.guid.str().c_str());
        });

        DrawComponent<TransformComponent>("Transform", selectedEntity, [](auto& component)
        {
            auto& position = component.GetLocalPosition();
            DrawVec3Control("Position", position);
            auto& rotation = component.GetRotation();
            DrawVec3Control("Rotation", rotation);
            auto& scale = component.GetScale();
            DrawVec3Control("Scale", scale);
        });

        DrawComponent<SkyboxComponent>("Skybox", selectedEntity, [this](auto& component)
        {
            auto& assetManager = AssetManager::Get();
            const auto& currentEnvironment = assetManager.GetAsset<EnvironmentContext>(component.environmentHandle);
            ImGui::LabelText("Image name", "%s", currentEnvironment->name.c_str());
            ImGui::Separator();
            ImGuiLayer::Image(currentEnvironment->equirectangularTexture, ImVec2(512, 256), ImVec2(0, 1), ImVec2(1, 0));

            fileDialog.SetTitle("Open new environment map");
            fileDialog.SetTypeFilters({ ".hdr" });
            if (ImGui::Button("Open"))
            {
                fileDialog.Open();
            }
            fileDialog.Display();
            if (fileDialog.HasSelected())
            {
                const auto filePath = fileDialog.GetSelected().string();
                // TODO: Add delimiters for other OSs
#ifdef R_WIN32
                const std::string delimiter = "\\";
#else
                const std::string delimiter = "/";
#endif
                const auto filename = String::Split(filePath, delimiter).back();
                const std::filesystem::path from = filePath;
                const std::filesystem::path to = Path::ConvertEnginePathToOSPath("/Assets/Textures/") + filename;
                R_CORE_TRACE("{0}", filename);
                try
                {
                    std::filesystem::copy(from, to, std::filesystem::copy_options::update_existing);
                }
                catch (std::exception& ex)
                {
                    R_CORE_ASSERT(false, ex.what());
                }
                fileDialog.ClearSelected();

                const auto id = String::Split(filename, ".").front();

                if (environmentMaps.find(id) == environmentMaps.end())
                {
                    const auto environmentHandle = AssetManager::Get().GetLoader<EnvironmentMapLoader>()->Load("/Assets/Textures/" + filename, true);
                    environmentMaps[id] = environmentHandle;
                }

                component.environmentHandle = environmentMaps[id];
                component.isDirty = true;
            }
        });

        DrawComponent<MeshComponent>("Mesh", selectedEntity, [](auto& component)
        {
            bool isVisible = component.IsVisible();
            ImGui::Checkbox("Is visible", &isVisible);
            component.SetVisibility(isVisible);
        });
    }
    ImGui::End();
}
