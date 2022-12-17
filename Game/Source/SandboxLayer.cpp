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
        std::shared_ptr<Camera> camera;
        std::shared_ptr<Entity> skyboxCube;
        ImVec2 viewportSize{ width, height };
        ImVec2 newViewportSize{ 0, 0 };
        uint32_t newEntityId{ 1 };
        PropertyPanel propertyPanel;
        MeshLoader meshLoader;
        AssetHandle backpackHandle;
        std::shared_ptr<Buffer> lightUniformBuffer;
        UIState uiState;
        AssetHandle environmentHandle;
        std::shared_ptr<ImGuiLayer> imGuiLayer;
        std::shared_ptr<Buffer> skyboxVertexBuffer;
        std::shared_ptr<SceneRenderer> renderer;

        bool isViewportHovered{ false };
        std::shared_ptr<Entity> selectedEntity;
        ImGuizmo::OPERATION gizmoType{ ImGuizmo::TRANSLATE };
    };

    static LayerSceneData sceneData;

    void AddTag(std::shared_ptr<Entity>& entity, const std::string& name = "")
    {
        TagComponent tag;
        tag.id = sceneData.newEntityId++;
        if (name.empty())
        {
            tag.name = "Entity" + std::to_string(tag.id);
        } else
        {
            tag.name = name;
        }
        entity->AddComponent<TagComponent>(tag);
    }

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
        AddTag(node);
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

    sceneData.camera = std::make_shared<Camera>(glm::vec3(0, 0, -5),
                                                glm::vec3(0, 1, 0));
    scene = Scene::Create();

    std::shared_ptr<Entity> gun = CreateTestSceneNode(scene, &sceneData.backpackHandle);
    gun->GetComponent<TagComponent>().name = "Gun";

    auto& gunTransform = gun->GetComponent<TransformComponent>();
    auto& textureData = gun->GetComponent<MeshComponent>().GetMaterial()->textureData;

    const auto& textureLoader = assetManager.GetLoader<TextureLoader>();
    textureLoader->LoadAsync(textureData.albedo, "/Assets/Textures/backpack_albedo.jpg");
    textureLoader->LoadAsync(textureData.normal, "/Assets/Textures/backpack_normal.png");
    textureLoader->LoadAsync(textureData.roughness, "/Assets/Textures/backpack_roughness.jpg");
    textureLoader->LoadAsync(textureData.metallic, "/Assets/Textures/backpack_metallic.jpg");
    textureLoader->LoadAsync(textureData.ao, "/Assets/Textures/backpack_ao.jpg");
    textureLoader->WaitAllLoaders();

    scene->SetCamera(sceneData.camera);
    scene->GetRootNode()->AddChild(gun);

    SamplerDescriptor samplerDesc{};
    const auto sampler = Device::Get()->CreateSampler(samplerDesc);

    sceneData.environmentHandle = assetManager.GetLoader<EnvironmentMapLoader>()->Load("/Assets/Textures/env_circus.hdr");

    sceneData.skyboxCube = scene->CreateEntity();
    sceneData.skyboxCube->AddComponent<TagComponent>(TagComponent("Skybox", sceneData.newEntityId++));
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
    light->GetComponent<TransformComponent>().SetPosition(glm::vec3(0, 10, -15));

    auto light1 = CreateTestSceneNode(scene, nullptr, GeometryType::NONE);
    light1->GetComponent<TagComponent>().name = "Light1";
    auto lightComponent1 = LightComponent();
    lightComponent1.intensity = 500.0f;
    lightComponent1.color = glm::vec3(1.0f, 1.0f, 1.0f);
    light1->AddComponent<LightComponent>(lightComponent);
    light1->GetComponent<TransformComponent>().SetPosition(glm::vec3(0, 10, 15));

    scene->GetRootNode()->AddChild(light);
    scene->GetRootNode()->AddChild(light1);

    const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
    int windowW, windowH;
    glfwGetFramebufferSize(window, &windowW, &windowH);


    EventDispatcher::Get().Subscribe(MouseMovedEvent::descriptor, EVENT_CALLBACK(SandboxLayer::OnEvent));

    sceneData.imGuiLayer = std::make_shared<ImGuiLayer>(sceneData.renderer->GetPass(PassType::UI));
    Application::Get().PushOverlay(sceneData.imGuiLayer);
    sceneData.camera->SetActive(false);
    sceneData.renderer->SetUIPassCallback([&](const std::shared_ptr<CommandBuffer>& cmd)
    {
       sceneData.imGuiLayer->Begin();
       OnImGuiRender();
       sceneData.imGuiLayer->End(cmd);
    });
}

void SandboxLayer::OnUpdate(float ts)
{
    if (sceneData.newViewportSize.x != 0 && sceneData.newViewportSize.y != 0)
    {
        sceneData.renderer->Resize(sceneData.newViewportSize.x, sceneData.newViewportSize.y);
        sceneData.newViewportSize = { 0, 0 };
    }

    sceneData.camera->SetActive(Input::IsMouseButtonDown(MouseButton::Right) && sceneData.isViewportHovered);

    if (sceneData.camera->IsActive())
    {
        if (Input::IsKeyDown(R_KEY_W))
        {
            sceneData.camera->Move(R_KEY_W);
        }
        if (Input::IsKeyDown(R_KEY_S))
        {
            sceneData.camera->Move(R_KEY_S);
        }
        if (Input::IsKeyDown(R_KEY_A))
        {
            sceneData.camera->Move(R_KEY_A);
        }
        if (Input::IsKeyDown(R_KEY_D))
        {
            sceneData.camera->Move(R_KEY_D);
        }
    }

    scene->OnUpdate(ts);

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
    sceneData.renderer->BeginScene(scene->GetCamera(),
                                  assetManager.GetAsset<EnvironmentContext>(sceneData.environmentHandle), lightData);

    for (auto& entity: scene->GetRegistry().view<MeshComponent>())
    {
        const auto& tag = scene->GetRegistry().get<TagComponent>(entity);
        const auto& transform = scene->GetRegistry().get<TransformComponent>(entity);
        const auto& meshComponent = scene->GetRegistry().get<MeshComponent>(entity);
        const auto& material = meshComponent.GetMaterial();
        if (!meshComponent.IsVisible())
        {
            continue;
        }

        sceneData.renderer->SubmitMeshNode(assetManager.GetAsset<MeshNode>(meshComponent.GetMesh()), material, transform.GetWorldTransformMatrix());
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

        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Camera options"))
            {
                sceneData.uiState.isCameraOptionsOpen = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (sceneData.uiState.isCameraOptionsOpen)
    {
        if (ImGui::Begin("Camera Options", &sceneData.uiState.isCameraOptionsOpen))
        {
            auto& camera = sceneData.camera;

            float speed = camera->GetMovementSpeed();
            ImGui::SliderFloat("Movement speed", &speed, 40.0f, 150.0f);
            camera->SetMovementSpeed(speed);

            float fov = camera->GetFOV();
            ImGui::SliderFloat("FOV", &fov, 30.0f, 100.0f);
            camera->SetFOV(fov);

            float zNear = camera->GetNear();
            ImGui::SliderFloat("Z Near", &zNear, 0.1f, 1.0f);
            camera->SetNear(zNear);

            float zFar = camera->GetFar();
            ImGui::SliderFloat("Z Far", &zFar, 10.0f, 1000.0f);
            camera->SetFar(zFar);

            std::array<const char*, 3> aspectRatios = { "16/9", "4/3", "Fit to window" };
            static const char* currentRatio = aspectRatios[2];
            if (ImGui::BeginCombo("Aspect ratio", currentRatio))
            {
                for (int i = 0; i < aspectRatios.size(); i++)
                {
                    bool isSelected = (currentRatio == aspectRatios[i]);
                    if (ImGui::Selectable(aspectRatios[i], isSelected))
                    {
                        currentRatio = aspectRatios[i];
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            if (currentRatio)
            {
                float newAspectRatio;
                if (std::strcmp(currentRatio, aspectRatios[0]) == 0)
                {
                    newAspectRatio = 16.0f / 9.0f;
                }
                if (std::strcmp(currentRatio, aspectRatios[1]) == 0)
                {
                    newAspectRatio = 4.0f / 3.0f;
                }
                if (std::strcmp(currentRatio, aspectRatios[2]) == 0)
                {
                    newAspectRatio = camera->GetAspectRatio();
                }
                camera->SetAspectRatio(newAspectRatio);
            }
        }
        ImGui::End();
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
            auto entity = scene->CreateEntity();
            entity->AddComponent<TagComponent>();
            scene->GetRootNode()->AddChild(entity);
        }

        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Viewport");
    sceneData.isViewportHovered = ImGui::IsWindowHovered();
    const auto attachment = sceneData.renderer->GetPass(PassType::PBR)->GetRenderPassDescriptor().colorAttachments.front().texture;
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
    sceneData.camera->SetAspectRatio(viewportSize.x / viewportSize.y);

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

        glm::mat4 cameraView = sceneData.camera->GetViewMatrix();
        glm::mat4 cameraProjection = sceneData.camera->GetProjectionMatrix();

        auto& entityTransformComponent = sceneData.selectedEntity->GetComponent<TransformComponent>();
        glm::mat4 entityTransform = entityTransformComponent.GetWorldTransformMatrix();

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                             (ImGuizmo::OPERATION) sceneData.gizmoType, ImGuizmo::LOCAL, glm::value_ptr(entityTransform));


        if (ImGuizmo::IsUsing())
        {
            glm::vec3 position, rotation, scale;
            Utils::DecomposeTransform(entityTransform, position, rotation, scale);

            auto deltaRotation = rotation - entityTransformComponent.GetRotation();
            entityTransformComponent.SetPosition(position);
            entityTransformComponent.SetRotationRadians(entityTransformComponent.GetRotation() + deltaRotation);
            entityTransformComponent.SetScale(scale);
        }
    }

    ImGui::End();

    sceneData.propertyPanel.OnImGuiRender();

    ImGui::End();
}

bool SandboxLayer::OnEvent(const Event& event)
{
    if (event.GetType() == MouseMovedEvent::descriptor)
    {
        MouseMovedEvent mouseMovedEvent = static_cast<const MouseMovedEvent&>(event);
        sceneData.camera->Rotate(mouseMovedEvent.GetX(), mouseMovedEvent.GetY());
        return true;
    }

    return false;
}
