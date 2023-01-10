#include "SandboxLayer.hpp"
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

    struct UIState
    {
        bool isCameraOptionsOpen{ true };
    };

    struct LayerSceneData
    {
        std::shared_ptr<Entity> skyboxCube;
        ImVec2 viewportSize{ width, height };
        ImVec2 newViewportSize{ 0, 0 };
        PropertyPanel propertyPanel;
        MeshLoader meshLoader;
        AssetHandle backpackHandle;
        std::shared_ptr<Buffer> lightUniformBuffer;
        UIState uiState;
        AssetHandle environmentHandle;
        std::shared_ptr<ImGuiLayer> imGuiLayer;
        std::shared_ptr<Buffer> skyboxVertexBuffer;
        std::shared_ptr<SceneRenderer> renderer;
        SceneRendererSettings rendererSettings;

        bool isViewportHovered{ false };
        std::shared_ptr<Entity> selectedEntity;
        ImGuizmo::OPERATION gizmoType{ ImGuizmo::TRANSLATE };
    };

    LayerSceneData sceneData;

    std::shared_ptr<Entity> CreateTestSceneNode(const std::shared_ptr<Scene>& scene,
                                                const AssetHandle* meshHandle = nullptr,
                                                GeometryType type = GeometryType::NONE)
    {
        std::shared_ptr<Entity> node = scene->CreateEntity();
        if (meshHandle)
        {
            MeshComponent meshComponent;
            meshComponent.SetMesh(*meshHandle);
            node->AddComponent<MeshComponent>(meshComponent);
        }
        return node;
    }

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

void SandboxLayer::OnAttach()
{
    sceneData.propertyPanel.Init();
    sceneData.renderer = std::make_shared<SceneRenderer>();
    sceneData.renderer->Init();

    auto& assetManager = AssetManager::Get();
    sceneData.backpackHandle = assetManager.GetLoader<MeshLoader>()->Load("/Assets/Models/backpack.obj");
    scene = Scene::Create();

    auto editorCamera = scene->CreateEntity("Editor camera", true);
    CameraComponent camera;
    camera.isActive = true;
    camera.isPrimary = true;
    editorCamera->AddComponent<CameraComponent>(camera);

    std::shared_ptr<Entity> backpack = CreateTestSceneNode(scene, &sceneData.backpackHandle);
    backpack->GetComponent<TagComponent>().name = "Backpack";
    auto& gunTransform = backpack->GetComponent<TransformComponent>();
    auto& backpackMesh = backpack->GetComponent<MeshComponent>();
    auto backpackMaterialRef = assetManager.GetLoader<MaterialLoader>()->Load();
    auto backpackMaterial = assetManager.GetAsset<Material>(backpackMaterialRef);

    const auto& textureLoader = assetManager.GetLoader<TextureLoader>();
    textureLoader->LoadAsync(backpackMaterial->textureData.albedo, "/Assets/Textures/backpack_albedo.jpg");
    textureLoader->LoadAsync(backpackMaterial->textureData.normal, "/Assets/Textures/backpack_normal.png");
    textureLoader->LoadAsync(backpackMaterial->textureData.roughness, "/Assets/Textures/backpack_roughness.jpg");
    textureLoader->LoadAsync(backpackMaterial->textureData.metallic, "/Assets/Textures/backpack_metallic.jpg");
    textureLoader->LoadAsync(backpackMaterial->textureData.ao, "/Assets/Textures/backpack_ao.jpg");
    textureLoader->WaitAllLoaders();

    backpackMesh.SetMaterial(backpackMaterialRef);

    scene->GetRootNode()->AddChild(backpack);

    SamplerDescriptor samplerDesc{};
    const auto sampler = Device::Get()->CreateSampler(samplerDesc);

    sceneData.environmentHandle = assetManager.GetLoader<EnvironmentMapLoader>()->Load("/Assets/Textures/env_circus.hdr");

    sceneData.skyboxCube = scene->CreateEntity("Skybox", true);
    auto& skyboxComponent = sceneData.skyboxCube->AddComponent<SkyboxComponent>();
    skyboxComponent.environmentHandle = sceneData.environmentHandle;
    scene->GetRootNode()->AddChild(sceneData.skyboxCube);

    sceneData.propertyPanel.SetScene(scene);

    auto light = CreateTestSceneNode(scene, nullptr, GeometryType::NONE);
    light->GetComponent<TagComponent>().name = "Light";
    auto lightComponent = LightComponent();
    lightComponent.intensity = 500.0f;
    lightComponent.color = glm::vec3(1.0f, 1.0f, 1.0f);
    light->AddComponent<LightComponent>(lightComponent);
    light->GetComponent<TransformComponent>().position = glm::vec3(0, 10, -15);

    auto light1 = CreateTestSceneNode(scene, nullptr, GeometryType::NONE);
    light1->GetComponent<TagComponent>().name = "Light1";
    auto lightComponent1 = LightComponent();
    lightComponent1.intensity = 500.0f;
    lightComponent1.color = glm::vec3(1.0f, 1.0f, 1.0f);
    light1->AddComponent<LightComponent>(lightComponent);
    light1->GetComponent<TransformComponent>().position = glm::vec3(0, 10, 15);

    scene->GetRootNode()->AddChild(light);
    scene->GetRootNode()->AddChild(light1);

    const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
    int windowW, windowH;
    glfwGetFramebufferSize(window, &windowW, &windowH);

    EventDispatcher::Get().Subscribe(MouseMovedEvent::descriptor, EVENT_CALLBACK(SandboxLayer::OnEvent));

    sceneData.imGuiLayer = std::make_shared<ImGuiLayer>(sceneData.renderer->GetPass(PassType::UI));
    Application::Get().PushOverlay(sceneData.imGuiLayer);
    sceneData.renderer->SetUIPassCallback([&](const std::shared_ptr<CommandBuffer>& cmd)
                                          {
                                              sceneData.imGuiLayer->Begin();
                                              OnImGuiRender();
                                              sceneData.imGuiLayer->End(cmd);
                                          });

    SceneSerializer serializer(scene);
    serializer.Serialize("/scene.yaml");
}

void SandboxLayer::OnUpdate(float ts)
{
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
        lightData.emplace_back(shaderLight);
    }

    auto& assetManager = AssetManager::Get();
    sceneData.renderer->SetScene(scene);
    sceneData.renderer->BeginScene(cameraData,
                                   assetManager.GetAsset<EnvironmentContext>(sceneData.environmentHandle),
                                   lightData,
                                   sceneData.rendererSettings);

    for (auto& entity: scene->GetRegistry().view<MeshComponent>())
    {
        const auto& tag = scene->GetRegistry().get<TagComponent>(entity);
        const auto& transform = scene->GetRegistry().get<TransformComponent>(entity);
        const auto& meshComponent = scene->GetRegistry().get<MeshComponent>(entity);
        const auto& materialRef = meshComponent.GetMaterial();
        if (!meshComponent.IsVisible())
        {
            continue;
        }

        sceneData.renderer->SubmitMeshNode(assetManager.GetAsset<MeshNode>(meshComponent.GetMesh()),
                                           assetManager.GetAsset<Material>(materialRef),
                                           transform.GetWorldTransformMatrix());
    }

    sceneData.renderer->EndScene();
}

void SandboxLayer::OnImGuiRender()
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

bool SandboxLayer::OnEvent(const Event& event)
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
