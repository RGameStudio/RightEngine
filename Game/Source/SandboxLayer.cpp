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
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <imgui.h>

using namespace RightEngine;

namespace
{
    float skyboxVertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,   // bottom-left
            1.0f, 1.0f, -1.0f,   // top-right
            1.0f, -1.0f, -1.0f,   // bottom-right
            1.0f, 1.0f, -1.0f,   // top-right
            -1.0f, -1.0f, -1.0f,   // bottom-left
            -1.0f, 1.0f, -1.0f,   // top-left
            // front face
            -1.0f, -1.0f, 1.0f,   // bottom-left
            1.0f, -1.0f, 1.0f,   // bottom-right
            1.0f, 1.0f, 1.0f,   // top-right
            1.0f, 1.0f, 1.0f,   // top-right
            -1.0f, 1.0f, 1.0f,   // top-left
            -1.0f, -1.0f, 1.0f,   // bottom-left
            // left face
            -1.0f, 1.0f, 1.0f,  // top-right
            -1.0f, 1.0f, -1.0f,  // top-left
            -1.0f, -1.0f, -1.0f,  // bottom-left
            -1.0f, -1.0f, -1.0f,  // bottom-left
            -1.0f, -1.0f, 1.0f,  // bottom-right
            -1.0f, 1.0f, 1.0f,  // top-right
            // right face
            1.0f, 1.0f, 1.0f,   // top-left
            1.0f, -1.0f, -1.0f,   // bottom-right
            1.0f, 1.0f, -1.0f,   // top-right
            1.0f, -1.0f, -1.0f,  // bottom-right
            1.0f, 1.0f, 1.0f,   // top-left
            1.0f, -1.0f, 1.0f,   // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,   // top-right
            1.0f, -1.0f, -1.0f,   // top-left
            1.0f, -1.0f, 1.0f,   // bottom-left
            1.0f, -1.0f, 1.0f,   // bottom-left
            -1.0f, -1.0f, 1.0f,   // bottom-right
            -1.0f, -1.0f, -1.0f,   // top-right
            // top face
            -1.0f, 1.0f, -1.0f,  // top-left
            1.0f, 1.0f, 1.0f,   // bottom-right
            1.0f, 1.0f, -1.0f,   // top-right
            1.0f, 1.0f, 1.0f,   // bottom-right
            -1.0f, 1.0f, -1.0f,   // top-left
            -1.0f, 1.0f, 1.0f,    // bottom-left
    };

    struct ShaderLight
    {
        glm::vec4 color;
        glm::vec4 position;
        glm::vec4 rotation;
        float intensity;
        int type;
        glm::vec2 _padding_;
    };

    struct LightBuffer
    {
        glm::ivec4 lightsAmount;
        ShaderLight light[30];
    };

    enum class TextureSlot
    {
        ALBEDO_TEXTURE_SLOT = 0,
        NORMAL_TEXTURE_SLOT,
        METALLIC_TEXTURE_SLOT,
        ROUGHNESS_TEXTURE_SLOT,
        AO_TEXTURE_SLOT,
        SKYBOX_TEXTURE_SLOT,
        PREFILTER_TEXTURE_SLOT,
        IRRADIANCE_TEXTURE_SLOT,
        BRDF_LUT_TEXTURE_SLOT
    };

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
        std::shared_ptr<Buffer> materialUniformBuffer;
        std::shared_ptr<Camera> camera;
        std::shared_ptr<Entity> skyboxCube;
        std::shared_ptr<Shader> skyboxShader;
        ImVec2 viewportSize{ width, height };
        uint32_t newEntityId{ 1 };
        PropertyPanel propertyPanel;
        MeshLoader meshLoader;
        AssetHandle backpackHandle;
        std::shared_ptr<Buffer> lightUniformBuffer;
        UIState uiState;
    };

    static LayerSceneData sceneData;


    void TryTextureBind(MaterialData& materialData, const std::shared_ptr<Texture>& texture, TextureSlot slot)
    {
        bool hasTexture = false;
        if (texture)
        {
            texture->Bind(static_cast<uint32_t>(slot));
            hasTexture = true;
        }

        switch (slot)
        {
            case TextureSlot::ALBEDO_TEXTURE_SLOT:
                materialData.hasAlbedo = hasTexture;
                break;
            case TextureSlot::NORMAL_TEXTURE_SLOT:
                materialData.hasNormal = hasTexture;
                break;
            case TextureSlot::METALLIC_TEXTURE_SLOT:
                materialData.hasMetallic = hasTexture;
                break;
            case TextureSlot::ROUGHNESS_TEXTURE_SLOT:
                materialData.hasRoughness = hasTexture;
                break;
            case TextureSlot::AO_TEXTURE_SLOT:
                materialData.hasAO = hasTexture;
                break;
            default:
            R_ASSERT(false, "Unknown texture slot!");
        }
    }

    void BindTextures(MaterialData& material, const TextureData& textures)
    {
//        TryTextureBind(material, textures.albedo, TextureSlot::ALBEDO_TEXTURE_SLOT);
//        TryTextureBind(material, textures.normal, TextureSlot::NORMAL_TEXTURE_SLOT);
//        TryTextureBind(material, textures.metallic, TextureSlot::METALLIC_TEXTURE_SLOT);
//        TryTextureBind(material, textures.roughness, TextureSlot::ROUGHNESS_TEXTURE_SLOT);
//        TryTextureBind(material, textures.ao, TextureSlot::AO_TEXTURE_SLOT);
    }

    void AddTag(std::shared_ptr<Entity>& entity, const std::string& name = "")
    {
        TagComponent tag;
        tag.id = sceneData.newEntityId++;
        if (name.empty())
        {
            tag.name = "Entity" + std::to_string(tag.id);
        }
        else
        {
            tag.name = name;
        }
        entity->AddComponent<TagComponent>(tag);
    }

    void CreateEntitiesFromMeshTree(std::shared_ptr<Entity>& entity,
                                    const std::shared_ptr<MeshNode>& tree,
                                    const std::shared_ptr<Scene>& scene)
    {
        if (!entity)
        {
            entity = scene->CreateEntity();
            AddTag(entity);
        }

        for (auto& mesh: tree->meshes)
        {
            auto newMeshEntity = scene->CreateEntity();
            AddTag(newMeshEntity);
            newMeshEntity->AddComponent<MeshComponent>(*mesh);
            entity->AddChild(newMeshEntity);
        }

        for (auto& child: tree->children)
        {
            if (child->meshes.empty())
            {
                continue;
            }
            auto newChild = scene->CreateEntity();
            AddTag(newChild);
            entity->AddChild(newChild);
            CreateEntitiesFromMeshTree(newChild, child, scene);
        }
    }

    std::shared_ptr<Entity> CreateTestSceneNode(const std::shared_ptr<Scene>& scene,
                                                const std::shared_ptr<MeshNode>& meshTree = nullptr,
                                                GeometryType type = GeometryType::NONE)
    {
        std::shared_ptr<Entity> node = scene->CreateEntity();
        if (meshTree)
        {
            CreateEntitiesFromMeshTree(node, meshTree, scene);
        }
        else
        {
            if (type != GeometryType::NONE)
            {
                std::shared_ptr<MeshComponent> mesh = nullptr;
                switch (type)
                {
                    case GeometryType::CUBE:
                        mesh = MeshBuilder::CubeGeometry();
                        break;
                    case GeometryType::PLANE:
                        mesh = MeshBuilder::PlaneGeometry();
                        break;
                }
                node->AddComponent<MeshComponent>(std::move(*mesh));
            }
        }
        AddTag(node);
        return node;
    }

    void ImGuiAddTreeNodeChildren(const std::shared_ptr<Entity>& node)
    {
        for (const auto& entity: node->GetChildren())
        {
            const auto& tag = entity->GetComponent<TagComponent>();
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
            bool node_open = ImGui::TreeNodeEx((void*) tag.id, node_flags, "%s", tag.name.c_str());
            if (ImGui::IsItemClicked())
            {
                sceneData.propertyPanel.SetSelectedEntity(entity);
            }
            if (node_open)
            {
                ImGuiAddTreeNodeChildren(entity);
                ImGui::TreePop();
            }
        }
    }
}

void SandboxLayer::OnAttach()
{
    auto& assetManager = AssetManager::Get();
    sceneData.backpackHandle = assetManager.GetLoader<MeshLoader>()->Load("/Assets/Models/backpack.obj");

    sceneData.camera = std::make_shared<Camera>(glm::vec3(0, 10, -15),
                                                      glm::vec3(0, 1, 0));
    scene = Scene::Create();

    std::shared_ptr<Entity> gun = CreateTestSceneNode(scene, assetManager.GetAsset<MeshNode>(sceneData.backpackHandle));
    gun->GetComponent<TagComponent>().name = "Gun";

    auto& gunMesh = gun->GetChildren().back();
    auto& gunTransform = gunMesh->GetComponent<TransformComponent>();
    gunTransform.SetScale({ 0.3f, 0.3f, 0.3f });
    gunTransform.SetRotation({ -90.0f, 0.0f, 0.0f });
    auto& textureData = gunMesh->GetComponent<MeshComponent>().GetMaterial()->textureData;

    const auto& textureLoader = assetManager.GetLoader<TextureLoader>();
    textureData.albedo = textureLoader->Load("/Assets/Textures/backpack_albedo.jpg");
    textureData.normal = textureLoader->Load("/Assets/Textures/backpack_normal.png");
    textureData.roughness = textureLoader->Load("/Assets/Textures/backpack_roughness.jpg");
    textureData.metallic = textureLoader->Load("/Assets/Textures/backpack_metallic.jpg");

    scene->SetCamera(sceneData.camera);
    scene->GetRootNode()->AddChild(gun);

    ShaderProgramDescriptor shaderProgramDescriptor;
    ShaderDescriptor vertexShader;
    vertexShader.path = "/Assets/Shaders/pbr.vert";
    vertexShader.type = ShaderType::VERTEX;
    ShaderDescriptor fragmentShader;
    fragmentShader.path = "/Assets/Shaders/pbr.frag";
    fragmentShader.type = ShaderType::FRAGMENT;
    shaderProgramDescriptor.shaders = {vertexShader, fragmentShader};
    VertexBufferLayout layout;
    layout.Push<glm::vec3>();
    layout.Push<glm::vec3>();
    layout.Push<glm::vec2>();
    layout.Push<glm::vec3>();
    layout.Push<glm::vec3>();
    shaderProgramDescriptor.layout = layout;
    shaderProgramDescriptor.reflection.textures = { 3, 4, 5, 6, 7, 8, 9, 10 };
    shaderProgramDescriptor.reflection.buffers[{ 0, ShaderType::VERTEX }] = BufferType::UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{ 1, ShaderType::VERTEX }] = BufferType::UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{ 2, ShaderType::FRAGMENT }] = BufferType::UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{ 11, ShaderType::FRAGMENT }] = BufferType::UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{ 12, ShaderType::FRAGMENT }] = BufferType::UNIFORM;
    const auto shader = Device::Get()->CreateShader(shaderProgramDescriptor);


    renderer = std::make_shared<Renderer>();

//    FramebufferSpecification fbSpec;
//    fbSpec.width = sceneData.viewportSize.x;
//    fbSpec.height = sceneData.viewportSize.y;
//    fbSpec.attachments = FramebufferAttachmentSpecification(
//            {
//                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8),
//                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8),
//                    FramebufferTextureSpecification(FramebufferTextureFormat::Depth),
//            }
//    );
//
//    frameBuffer = std::make_shared<Framebuffer>(fbSpec);
//
//    sceneData.materialUniformBuffer = UniformBuffer::Create(sizeof(MaterialData), 0);
//    sceneData.lightUniformBuffer = UniformBuffer::Create(sizeof(LightBuffer), 1);
//
//    sceneData.skyboxShader = assetManager.LoadAsset<Shader>(
//            "/Assets/Shaders/Basic/skybox",
//            "skybox",
//            options);
//
//    options.flipTextureVertically = true;
//    const auto envContext = assetManager.LoadAsset<EnvironmentContext>("/Assets/Textures/env_malibu.hdr", "env_malibu",
//                                                                       options);

//    sceneData.skyboxCube = scene->CreateEntity();
//    VertexBufferLayout layout;
//    layout.Push<float>(3);
//    MeshComponent mesh;
//    auto vertexArray = std::make_shared<VertexArray>();
//    vertexArray->AddBuffer(std::make_shared<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices)), layout);
//    mesh.SetVertexArray(vertexArray);
//    mesh.SetVisibility(false);
//    sceneData.skyboxCube->AddComponent<MeshComponent>(mesh);
//    sceneData.skyboxCube->AddComponent<TagComponent>(TagComponent("Skybox", sceneData.newEntityId++));
//    auto& skyboxComponent = sceneData.skyboxCube->AddComponent<SkyboxComponent>();
//    skyboxComponent.environment = envContext;
//    scene->GetRootNode()->AddChild(sceneData.skyboxCube);

    sceneData.propertyPanel.SetScene(scene);

    auto light = CreateTestSceneNode(scene, nullptr, GeometryType::NONE);
    light->GetComponent<TagComponent>().name = "Light";
    auto lightComponent = LightComponent();
    lightComponent.intensity = 1000.0f;
    lightComponent.color = glm::vec3(1.0f, 0.0f, 0.0f);
    light->AddComponent<LightComponent>(lightComponent);
    light->GetComponent<TransformComponent>().SetPosition(glm::vec3(0, 10, -15));

    auto light1 = CreateTestSceneNode(scene, nullptr, GeometryType::NONE);
    light1->GetComponent<TagComponent>().name = "Light1";
    auto lightComponent1 = LightComponent();
    lightComponent1.intensity = 1000.0f;
    lightComponent1.color = glm::vec3(1.0f, 0.0f, 0.0f);
    light1->AddComponent<LightComponent>(lightComponent);
    light1->GetComponent<TransformComponent>().SetPosition(glm::vec3(0, 10, 15));

    scene->GetRootNode()->AddChild(light);
    scene->GetRootNode()->AddChild(light1);

    R_CORE_ASSERT(false, "");
}

void SandboxLayer::OnUpdate(float ts)
{
//    scene->OnUpdate();
//
//    frameBuffer->Bind();
//    RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    renderer->Configure();
//    renderer->BeginScene(scene);
//
//    LightBuffer lightBuffer;
//    std::memset(&lightBuffer, 0, sizeof(LightBuffer));
//    for (const auto& entityID: scene->GetRegistry().view<LightComponent>())
//    {
//        const auto& transform = scene->GetRegistry().get<TransformComponent>(entityID);
//        const auto& light = scene->GetRegistry().get<LightComponent>(entityID);
//        ShaderLight shaderLight;
//        shaderLight.type = static_cast<int>(light.type);
//        shaderLight.position = glm::vec4(transform.GetWorldPosition(), 1);
//        shaderLight.color = glm::vec4(light.color, 1);
//        shaderLight.intensity = light.intensity;
//        lightBuffer.light[lightBuffer.lightsAmount.x] = shaderLight;
//        lightBuffer.lightsAmount.x += 1;
//        switch (light.type)
//        {
//            case LightType::DIRECTIONAL:
//                lightBuffer.lightsAmount.y += 1;
//                break;
//            default:
//            R_ASSERT(false, "");
//        }
//    }
//
//    R_ASSERT(lightBuffer.lightsAmount.x < 30, "");
//    sceneData.lightUniformBuffer->SetData(&lightBuffer, sizeof(LightBuffer));
//
//    const auto skyboxView = scene->GetRegistry().view<SkyboxComponent>();
//    // TODO: Add black skybox for fallback
//    R_ASSERT(!skyboxView.empty(), "No skybox was set!");
//    R_ASSERT(skyboxView.size() == 1, "There must only 1 skybox in scene!");
//    const auto& skyboxEntityID = skyboxView.front();
//    const auto& skybox = scene->GetRegistry().get<SkyboxComponent>(skyboxEntityID);
//
//    for (const auto& entity: scene->GetRegistry().view<MeshComponent>())
//    {
//        const auto& transform = scene->GetRegistry().get<TransformComponent>(entity);
//        const auto& mesh = scene->GetRegistry().get<MeshComponent>(entity);
//        if (!mesh.IsVisible())
//        {
//            continue;
//        }
//
//        auto& materialData = mesh.GetMaterial()->materialData;
//        const auto& textureData = mesh.GetMaterial()->textureData;
//        BindTextures(materialData, textureData);
//
//        shader->Bind();
//        shader->SetUniform1iv("u_Textures", { 0, 1, 2, 3, 4 });
//        shader->SetUniform3f("u_CameraPosition", sceneData.camera->GetPosition());
//        skybox.environment->irradianceMap->Bind(static_cast<uint32_t>(TextureSlot::IRRADIANCE_TEXTURE_SLOT));
//        shader->SetUniform1i("u_IrradianceMap", static_cast<uint32_t>(TextureSlot::IRRADIANCE_TEXTURE_SLOT));
//        skybox.environment->prefilterMap->GetSampler()->Bind(
//                static_cast<uint32_t>(TextureSlot::PREFILTER_TEXTURE_SLOT));
//        skybox.environment->prefilterMap->Bind(static_cast<uint32_t>(TextureSlot::PREFILTER_TEXTURE_SLOT));
//        shader->SetUniform1i("u_PrefilterMap", static_cast<uint32_t>(TextureSlot::PREFILTER_TEXTURE_SLOT));
//        skybox.environment->brdfLut->GetSampler()->Bind(static_cast<uint32_t>(TextureSlot::BRDF_LUT_TEXTURE_SLOT));
//        skybox.environment->brdfLut->Bind(static_cast<uint32_t>(TextureSlot::BRDF_LUT_TEXTURE_SLOT));
//        shader->SetUniform1i("u_BRDFLUT", static_cast<uint32_t>(TextureSlot::BRDF_LUT_TEXTURE_SLOT));
//
//        sceneData.materialUniformBuffer->SetData(&materialData, sizeof(MaterialData));
//        renderer->SubmitMesh(shader, mesh, transform.GetWorldTransformMatrix());
//    }
//    shader->UnBind();
//    frameBuffer->UnBind();
//
//    frameBuffer->Bind();
//    sceneData.skyboxShader->Bind();
//    skybox.environment->envMap->Bind(static_cast<uint32_t>(TextureSlot::SKYBOX_TEXTURE_SLOT));
//    sceneData.skyboxShader->SetUniform1i("u_Skybox", static_cast<uint32_t>(TextureSlot::SKYBOX_TEXTURE_SLOT));
//    const auto projectionMatrix = glm::perspective(sceneData.camera->GetFOV(true),
//                                                   sceneData.camera->GetAspectRatio(),
//                                                   sceneData.camera->GetNear(),
//                                                   sceneData.camera->GetFar());
//    const auto viewMatrix = glm::mat4(glm::mat3(scene->GetCamera()->GetViewMatrix()));
//    sceneData.skyboxShader->SetUniformMat4f("u_Projection", projectionMatrix);
//    sceneData.skyboxShader->SetUniformMat4f("u_View", viewMatrix);
//    auto& rendererSettings = renderer->GetSettings();
//    rendererSettings.depthTestMode = RightEngine::DepthTestMode::LEQUAL;
//    renderer->Configure();
//    renderer->SubmitMesh(sceneData.skyboxShader,
//                         sceneData.skyboxCube->GetComponent<MeshComponent>(),
//                         sceneData.skyboxCube->GetComponent<TransformComponent>().GetWorldTransformMatrix());
//    rendererSettings.depthTestMode = RightEngine::DepthTestMode::LESS;
//    renderer->Configure();
//    frameBuffer->UnBind();
//    renderer->EndScene();
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
    }
    else
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
        ImGuiAddTreeNodeChildren(node);
        ImGui::TreePop();
        ImGui::PopStyleVar();
    }
    ImGui::End();

    ImGui::Begin("Viewport");
//    id = frameBuffer->GetColorAttachment();
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    if (viewportSize.x != sceneData.viewportSize.x || viewportSize.y != sceneData.viewportSize.y)
    {
        if (viewportSize.y <= 0)
        {
            viewportSize.y = 1;
        }
        // TODO: Fix image flickering while resizing window
//        frameBuffer->Resize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
        sceneData.viewportSize = viewportSize;
    }
    ImGui::Image((void*) id, sceneData.viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    sceneData.camera->SetAspectRatio(viewportSize.x / viewportSize.y);
    ImGui::End();

    sceneData.propertyPanel.OnImGuiRender();

    ImGui::End();
}
