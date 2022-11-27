#include "PropertyPanel.hpp"
#include "Components.hpp"
#include "String.hpp"
#include "Path.hpp"
#include "AssetManager.hpp"
#include "ImGuiLayer.hpp"
#include "Filesystem.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <filesystem>

using namespace RightEngine;

namespace
{
    AssetHandle editorDefaultTexture;
    const std::string texturesDir = "/Assets/Textures/";
    bool isDisplayCalled = false;

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

    std::string CopyFileToAssetDirectory(const std::string& filePath, const std::string& assetDirectory)
    {
        auto filename = String::Split(filePath, "/").back();
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

        return filename;
    }

    std::string OpenFileDialogOnItemClick(const std::string& title,
                                          const std::vector<std::string>& filters,
                                          const std::string& assetDir)
    {
//        fileDialog.SetTitle(title);
//        fileDialog.SetTypeFilters(filters);
        if (ImGui::IsItemClicked())
        {
//            fileDialog.Open();
        }

//        fileDialog.Display();

//        if (fileDialog.HasSelected())
//        {
//            auto filename = CopyFileToAssetDirectory(fileDialog.GetSelected().string(), assetDir);
//            fileDialog.ClearSelected();
//            return filename;
//        }

        return "";
    }

    void DrawMaterialEditorTab(const std::string& label,
                               bool hasTexture,
                               AssetHandle& textureHandle,
                               std::unordered_map<std::string, AssetHandle>& textures,
                               MeshComponent& component)
    {
        auto& assetManager = AssetManager::Get();
        ImGui::LabelText("", label.c_str());
        if (hasTexture)
        {
            ImGuiLayer::Image(assetManager.GetAsset<Texture>(textureHandle), ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1))
            {
                textureHandle = {};
            }
        }
        else
        {
            ImGuiLayer::Image(assetManager.GetAsset<Texture>(editorDefaultTexture), ImVec2(64, 64));
        }

        const auto filename = OpenFileDialogOnItemClick("Choose texture", { ".png", ".jpg" }, texturesDir);
        if (!filename.empty())
        {
            const auto id = String::Split(filename, ".").front();

            if (textures.find(id) == textures.end())
            {
                const auto newTextureHandle = AssetManager::Get().GetLoader<TextureLoader>()->Load(texturesDir + filename);
                textures[id] = newTextureHandle;
            }

            textureHandle = textures[id];
            component.SetDirty(true);
        }
    }
}

PropertyPanel::PropertyPanel(const std::shared_ptr<Scene>& aScene)
{
    scene = aScene;
}

void PropertyPanel::Init()
{
    editorDefaultTexture = AssetManager::Get().GetLoader<TextureLoader>()->Load("/Assets/Textures/editor_default_texture.png");
    AssetManager::Get().GetAsset<Texture>(editorDefaultTexture)->SetSampler(Device::Get()->CreateSampler({}));
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
    isDisplayCalled = false;
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
            auto rotation = glm::degrees(component.GetRotation());
            DrawVec3Control("Rotation", rotation);
            component.SetRotationDegree(rotation);
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

            if (ImGui::Button("Open"))
            {
                const auto filepath = Filesystem::OpenFileDialog().string();
                if (filepath.empty())
                {
                    return;
                }
                const auto filename = CopyFileToAssetDirectory(filepath, texturesDir);
                const auto id = String::Split(filename, ".").front();

                if (environmentMaps.find(id) == environmentMaps.end())
                {
                    const auto environmentHandle = AssetManager::Get().GetLoader<EnvironmentMapLoader>()->Load(texturesDir + filename, true);
                    environmentMaps[id] = environmentHandle;
                }

                component.environmentHandle = environmentMaps[id];
                component.isDirty = true;
            }
        });

        DrawComponent<MeshComponent>("Mesh", selectedEntity, [this](auto& component)
        {
            auto& assetManager = AssetManager::Get();
            ImGui::LabelText("Mesh GUID", "%s", component.GetMesh().guid.str().c_str());
            ImGui::Separator();
            bool isVisible = component.IsVisible();
            ImGui::Checkbox("Is visible", &isVisible);
            component.SetVisibility(isVisible);
            ImGui::Separator();

//            fileDialog.SetTitle("Open new mesh");
//            fileDialog.SetTypeFilters({ ".obj", ".fbx", ".gltf" });
//            if (ImGui::Button("Open"))
//            {
//                fileDialog.Open();
//            }
//            fileDialog.Display();
//            if (fileDialog.HasSelected())
//            {
//                const auto assetDir = "/Assets/Models/";
//                const auto filename = CopyFileToAssetDirectory(fileDialog.GetSelected().string(), assetDir);
//                fileDialog.ClearSelected();
//
//                const auto id = String::Split(filename, ".").front();
//
//                if (meshes.find(id) == meshes.end())
//                {
//                    const auto meshHandle = AssetManager::Get().GetLoader<MeshLoader>()->Load(assetDir + filename);
//                    meshes[id] = meshHandle;
//                }
//
//                component.SetMesh(meshes[id]);
//            }

            ImGui::Separator();
            if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
            {
                auto& material = component.GetMaterial();
                ImGui::TableNextColumn();
                bool hasAlbedo =  material->materialData.hasAlbedo;
                DrawMaterialEditorTab("Albedo", hasAlbedo, material->textureData.albedo, textures, component);
                ImGui::TableNextColumn();
                ImGui::ColorEdit4("", &material->materialData.albedo.x);

                ImGui::TableNextColumn();
                bool hasNormal =  material->materialData.hasNormal;
                DrawMaterialEditorTab("Normal", hasNormal, material->textureData.normal, textures, component);
                ImGui::TableNextColumn();


                ImGui::EndTable();
            }
        });
    }
    ImGui::End();
}
