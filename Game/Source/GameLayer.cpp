#include "GameLayer.hpp"
#include "Renderer.hpp"
#include "RendererCommand.hpp"
#include "Entity.hpp"
#include "Components.hpp"
#include "Texture.hpp"
#include "TextureLoader.hpp"
#include "Panels/PropertyPanel.hpp"
#include "EnvironmentMapLoader.hpp"
#include "MeshLoader.hpp"
#include "AssetManager.hpp"
#include "KeyCodes.hpp"
#include "SceneRenderer.hpp"
#include "MouseEvent.hpp"
#include "SceneSerializer.hpp"
#include "MaterialLoader.hpp"
#include "Filesystem.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

using namespace RightEngine;

namespace
{
    enum class GeometryType
    {
        NONE,
        CUBE,
        PLANE
    };

    const uint32_t width = 1280;
    const uint32_t height = 720;

    struct LayerSceneData
    {
        ImVec2 viewportSize{ width, height };
        ImVec2 newViewportSize{ 0, 0 };
        PropertyPanel propertyPanel;
        std::shared_ptr<ImGuiLayer> imGuiLayer;
        std::shared_ptr<SceneRenderer> renderer;
        SceneRendererSettings rendererSettings;
        bool isViewportHovered{ false };
        std::shared_ptr<Entity> selectedEntity;
        ImGuizmo::OPERATION gizmoType{ ImGuizmo::TRANSLATE };
    };

    LayerSceneData sceneData;

    void ImGuiAddTreeNodeChildren(const std::shared_ptr<Entity>& node, const std::shared_ptr<Scene>& scene)
    {
        for (const auto& entity: node->GetChildren())
        {
            const auto& tag = entity->GetComponent<TagComponent>();
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
            bool node_open = ImGui::TreeNodeEx((char*) tag.guid.str().c_str(), node_flags, "%s", tag.name.c_str());
            if (ImGui::IsItemClicked())
            {
                sceneData.propertyPanel.SetSelectedEntity(entity);
                sceneData.selectedEntity = entity;
            }

            bool destroyEntity = false;
            if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight))
            {
                if (ImGui::MenuItem("Destroy Entity"))
                {
                    destroyEntity = true;
                }
                ImGui::EndPopup();
            }

            if (node_open)
            {
                ImGuiAddTreeNodeChildren(entity, scene);
                ImGui::TreePop();
            }

            if (destroyEntity)
            {
                const auto children = entity->GetChildren();
                const auto parentNode = entity->GetParent();
                for (const auto& child: children)
                {
                    parentNode->AddChild(child);
                }

                scene->DestroyEntity(entity);
                parentNode->RemoveChild(entity);
                sceneData.propertyPanel.SetSelectedEntity(nullptr);
                sceneData.selectedEntity = nullptr;
                return;
            }
        }
    }
}

void GameLayer::OnAttach()
{
    sceneData.propertyPanel.Init();
    sceneData.renderer = std::make_shared<SceneRenderer>();
    sceneData.renderer->Init();

    const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
    int windowW, windowH;
    glfwGetFramebufferSize(window, &windowW, &windowH);
    EventDispatcher::Get().Subscribe(MouseMovedEvent::descriptor, EVENT_CALLBACK(GameLayer::OnEvent));
    sceneData.imGuiLayer = std::make_shared<ImGuiLayer>(sceneData.renderer->GetPass(PassType::UI));
    Application::Get().PushOverlay(sceneData.imGuiLayer);
    sceneData.renderer->SetUIPassCallback([&](const std::shared_ptr<CommandBuffer>& cmd)
                                          {
                                              sceneData.imGuiLayer->Begin();
                                              OnImGuiRender();
                                              sceneData.imGuiLayer->End(cmd);
                                          });

    LoadDefaultScene();
}

void GameLayer::OnUpdate(float ts)
{
    if (newScene)
    {
        scene = newScene;
        newScene = nullptr;
    }
    if (sceneData.newViewportSize.x != 0 && sceneData.newViewportSize.y != 0)
    {
        sceneData.renderer->Resize(sceneData.newViewportSize.x, sceneData.newViewportSize.y);
        sceneData.newViewportSize = { 0, 0 };
    }

    scene->OnUpdate(ts);

    CameraData cameraData{};
    for (const auto eCamera : scene->GetRegistry().view<CameraComponent>())
    {
        auto& camera = scene->GetRegistry().get<CameraComponent>(eCamera);
        auto& transform = scene->GetRegistry().get<TransformComponent>(eCamera);
        camera.Rotate(glm::degrees(transform.rotation));
        camera.OnUpdate(ts);
        if (camera.isPrimary)
        {
            camera.isActive = Input::IsMouseButtonDown(MouseButton::Right) && sceneData.isViewportHovered;
            if (camera.isActive)
            {
                glm::vec3 position = transform.position;
                if (Input::IsKeyDown(R_KEY_W))
                {
                    position = camera.Move(R_KEY_W, position);
                }
                if (Input::IsKeyDown(R_KEY_S))
                {
                    position = camera.Move(R_KEY_S, position);
                }
                if (Input::IsKeyDown(R_KEY_A))
                {
                    position = camera.Move(R_KEY_A, position);
                }
                if (Input::IsKeyDown(R_KEY_D))
                {
                    position = camera.Move(R_KEY_D, position);
                }
                transform.position = position;
            }
            cameraData.position = transform.GetWorldPosition();
            cameraData.view = camera.GetViewMatrix(cameraData.position);
            cameraData.projection = camera.GetProjectionMatrix();
        }
    }

    std::vector<LightData> lightData;
    for (const auto& entityID: scene->GetRegistry().view<LightComponent>())
    {
        const auto& transform = scene->GetRegistry().get<TransformComponent>(entityID);
        const auto& light = scene->GetRegistry().get<LightComponent>(entityID);
        LightData shaderLight{};
        shaderLight.type = static_cast<int>(light.type);
        shaderLight.position = glm::vec4(transform.GetWorldPosition(), 1);
        shaderLight.color = glm::vec4(light.color, 1);
        shaderLight.intensity = light.intensity;
        shaderLight.radiusInner = light.innerRadius;
        shaderLight.radiusOuter = light.outerRadius;
        lightData.emplace_back(shaderLight);
    }

    auto& assetManager = AssetManager::Get();

    AssetHandle environmentHandle;
    for (const auto& entityID: scene->GetRegistry().view<SkyboxComponent>())
    {
        R_CORE_ASSERT(!environmentHandle.guid.isValid(), "")
        const auto& skybox = scene->GetRegistry().get<SkyboxComponent>(entityID);
        environmentHandle = skybox.environmentHandle;
        R_CORE_ASSERT(environmentHandle.guid.isValid(), "")
    }

    sceneData.renderer->SetScene(scene);
    sceneData.renderer->BeginScene(cameraData,
                                   assetManager.GetAsset<EnvironmentContext>(environmentHandle),
                                   lightData,
                                   sceneData.rendererSettings);

    for (auto& entity: scene->GetRegistry().view<MeshComponent>())
    {
        const auto& tag = scene->GetRegistry().get<TagComponent>(entity);
        const auto& transform = scene->GetRegistry().get<TransformComponent>(entity);
        const auto& meshComponent = scene->GetRegistry().get<MeshComponent>(entity);
        const auto& materialRef = meshComponent.material;
        if (!meshComponent.isVisible)
        {
            continue;
        }

        sceneData.renderer->SubmitMeshNode(assetManager.GetAsset<MeshNode>(meshComponent.mesh),
                                           assetManager.GetAsset<Material>(materialRef),
                                           transform.GetWorldTransformMatrix());
    }

    sceneData.renderer->EndScene();
}

void GameLayer::OnImGuiRender()
{
    bool dockspaceOpen = true;
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar(3);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
    } else
    {
        R_ASSERT(false, "");
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                EventDispatcher::Get().Emit(ShutdownEvent());
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("Save"))
            {
                SceneSerializer serializer(scene);
                serializer.Serialize(scene->GetName() + ".yaml");
            }
            if (ImGui::MenuItem("Load"))
            {
                SceneSerializer serializer(Scene::Create());
                const auto path = Filesystem::OpenFileDialog({ ".yaml" });
                if (!path.empty())
                {
                    serializer.Deserialize(path.generic_u8string());
                }
                newScene = serializer.GetScene();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::Begin("Scene Hierarchy");
    if (ImGui::TreeNodeEx("Root", ImGuiTreeNodeFlags_OpenOnArrow))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3);
        const auto& node = scene->GetRootNode();
        ImGuiAddTreeNodeChildren(node, scene);
        ImGui::TreePop();
        ImGui::PopStyleVar();
    }

    if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
    {
        if (ImGui::MenuItem("Create Empty Entity"))
        {
            auto entity = scene->CreateEntity("New entity", true);
        }

        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Viewport");
    sceneData.isViewportHovered = ImGui::IsWindowHovered();
    const auto attachment = sceneData.renderer->GetFinalImage();
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    if (viewportSize.x != sceneData.viewportSize.x || viewportSize.y != sceneData.viewportSize.y)
    {
        if (viewportSize.y <= 0)
        {
            viewportSize.y = 1;
        }
        // TODO: Fix image flickering while resizing window
        sceneData.newViewportSize = viewportSize;
        sceneData.viewportSize = viewportSize;
    }
    ImGuiLayer::Image(attachment, sceneData.viewportSize);
//    sceneData.camera->SetAspectRatio(viewportSize.x / viewportSize.y);

    if (sceneData.selectedEntity)
    {
        if (sceneData.isViewportHovered)
        {
            if (Input::IsKeyDown(R_KEY_T))
            {
                sceneData.gizmoType = ImGuizmo::TRANSLATE;
            } else if (Input::IsKeyDown(R_KEY_R))
            {
                sceneData.gizmoType = ImGuizmo::ROTATE;
            } else if (Input::IsKeyDown(R_KEY_B))
            {
                sceneData.gizmoType = ImGuizmo::SCALE;
            }
        }
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

        CameraComponent camera{};
        glm::vec3 cameraPos;
        for (const auto& eCamera : scene->GetRegistry().view<CameraComponent>())
        {
            auto& cameraComp = scene->GetRegistry().get<CameraComponent>(eCamera);
            auto& transform = scene->GetRegistry().get<TransformComponent>(eCamera);
            if (cameraComp.isPrimary)
            {
                camera = cameraComp;
                cameraPos = transform.GetWorldPosition();
                break;
            }
        }

        glm::mat4 cameraView = camera.GetViewMatrix(cameraPos);
        glm::mat4 cameraProjection = camera.GetProjectionMatrix();

        auto& entityTransformComponent = sceneData.selectedEntity->GetComponent<TransformComponent>();
        glm::mat4 entityTransform = entityTransformComponent.GetWorldTransformMatrix();

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                             (ImGuizmo::OPERATION) sceneData.gizmoType, ImGuizmo::LOCAL, glm::value_ptr(entityTransform));


        if (ImGuizmo::IsUsing())
        {
            glm::vec3 position, rotation, scale;
            Utils::DecomposeTransform(entityTransform, position, rotation, scale);

            auto deltaRotation = rotation - entityTransformComponent.rotation;
            entityTransformComponent.position = position;
            entityTransformComponent.SetRotationRadians(entityTransformComponent.rotation + deltaRotation);
            entityTransformComponent.scale = scale;
        }
    }

    ImGui::End();

    sceneData.propertyPanel.OnImGuiRender();

    ImGui::Begin("Renderer settings");
    ImGui::DragFloat("Gamma", &sceneData.rendererSettings.gamma, 0.1, 1.0, 3.2);
    ImGui::End();

    ImGui::End();
}

bool GameLayer::OnEvent(const Event& event)
{
    if (event.GetType() == MouseMovedEvent::descriptor)
    {
        MouseMovedEvent mouseMovedEvent = static_cast<const MouseMovedEvent&>(event);
        for (const auto& eCamera : scene->GetRegistry().view<CameraComponent>())
        {
            auto& camera = scene->GetRegistry().get<CameraComponent>(eCamera);
            auto& transform = scene->GetRegistry().get<TransformComponent>(eCamera);
            if (camera.isPrimary)
            {
                auto rotation = camera.Rotate(mouseMovedEvent.GetX(), mouseMovedEvent.GetY(), glm::degrees(transform.rotation));
                transform.SetRotationRadians(glm::radians(rotation));
                break;
            }
        }
        return true;
    }

    return false;
}

void GameLayer::LoadDefaultScene()
{
    SceneSerializer serializer(Scene::Create());
    R_CORE_ASSERT(serializer.Deserialize(DEFAULT_SCENE_PATH), "");
    scene = serializer.GetScene();
}
