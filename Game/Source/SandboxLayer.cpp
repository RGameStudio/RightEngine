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
        bool isCameraOptionsOpen{true};
    };

    struct VPBuffer
    {
        glm::mat4 viewProjection;
    };

    struct TransformBuffer
    {
        glm::mat4 transform;
    };

    struct CameraPosBuffer
    {
        glm::vec4 pos;
    };

    struct LayerSceneData
    {
        std::shared_ptr<GraphicsPipeline> pbrPipeline;
        std::shared_ptr<RendererState> pbrPipelineState;
        std::shared_ptr<GraphicsPipeline> skyboxPipeline;
        std::shared_ptr<GraphicsPipeline> presentPipeline;
        std::shared_ptr<GraphicsPipeline> uiPipeline;
        std::shared_ptr<RendererState> skyboxPipelineState;
        std::shared_ptr<Buffer> materialUniformBuffer;
        std::shared_ptr<Camera> camera;
        std::shared_ptr<Entity> skyboxCube;
        ImVec2 viewportSize{width, height};
        ImVec2 newViewportSize{0, 0};
        uint32_t newEntityId{1};
        PropertyPanel propertyPanel;
        MeshLoader meshLoader;
        AssetHandle backpackHandle;
        std::shared_ptr<Buffer> lightUniformBuffer;
        UIState uiState;
        AssetHandle environmentHandle;
        std::shared_ptr<Buffer> vpBuffer;
        std::shared_ptr<Buffer> transBuffer;
        std::shared_ptr<Buffer> cameraPosBuffer;
        std::shared_ptr<ImGuiLayer> imGuiLayer;
        std::shared_ptr<Buffer> skyboxVertexBuffer;
    };

    static LayerSceneData sceneData;


    void TryTextureBind(MaterialData& materialData, const AssetHandle& textureHandle, TextureSlot slot)
    {
        bool hasTexture = false;
        if (textureHandle.guid.isValid())
        {
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
        TryTextureBind(material, textures.albedo, TextureSlot::ALBEDO_TEXTURE_SLOT);
        TryTextureBind(material, textures.normal, TextureSlot::NORMAL_TEXTURE_SLOT);
        TryTextureBind(material, textures.metallic, TextureSlot::METALLIC_TEXTURE_SLOT);
        TryTextureBind(material, textures.roughness, TextureSlot::ROUGHNESS_TEXTURE_SLOT);
        TryTextureBind(material, textures.ao, TextureSlot::AO_TEXTURE_SLOT);
    }

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
        } else
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

    sceneData.camera = std::make_shared<Camera>(glm::vec3(0, 0, -5),
                                                glm::vec3(0, 1, 0));
    scene = Scene::Create();

    std::shared_ptr<Entity> gun = CreateTestSceneNode(scene, assetManager.GetAsset<MeshNode>(sceneData.backpackHandle));
    gun->GetComponent<TagComponent>().name = "Gun";

    auto& gunMesh = gun->GetChildren().back();
    auto& gunTransform = gunMesh->GetComponent<TransformComponent>();
    auto& textureData = gunMesh->GetComponent<MeshComponent>().GetMaterial()->textureData;

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
    shaderProgramDescriptor.reflection.textures = {3, 4, 5, 6, 7, 8, 9, 10};
    shaderProgramDescriptor.reflection.buffers[{0, ShaderType::VERTEX}] = BufferType::UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{1, ShaderType::VERTEX}] = BufferType::UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{2, ShaderType::FRAGMENT}] = BufferType::UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{11, ShaderType::FRAGMENT}] = BufferType::UNIFORM;
    shaderProgramDescriptor.reflection.buffers[{12, ShaderType::FRAGMENT}] = BufferType::UNIFORM;
    auto shader = Device::Get()->CreateShader(shaderProgramDescriptor);

    renderer = std::make_shared<Renderer>();

    TextureDescriptor colorAttachmentDesc{};
    colorAttachmentDesc.format = Format::BGRA8_SRGB;
    colorAttachmentDesc.type = TextureType::TEXTURE_2D;
    colorAttachmentDesc.width = sceneData.viewportSize.x;
    colorAttachmentDesc.height = sceneData.viewportSize.y;
    const auto colorAttachment = Device::Get()->CreateTexture(colorAttachmentDesc, {});
    colorAttachment->SetSampler(sampler);
    TextureDescriptor normalAttachmentDesc{};
    normalAttachmentDesc.format = Format::RGBA16_SFLOAT;
    normalAttachmentDesc.type = TextureType::TEXTURE_2D;
    normalAttachmentDesc.width = sceneData.viewportSize.x;
    normalAttachmentDesc.height = sceneData.viewportSize.y;
    const auto normalAttachemnt = Device::Get()->CreateTexture(normalAttachmentDesc, {});
    TextureDescriptor depthAttachmentDesc{};
    depthAttachmentDesc.format = Format::D32_SFLOAT_S8_UINT;
    depthAttachmentDesc.type = TextureType::TEXTURE_2D;
    depthAttachmentDesc.width = sceneData.viewportSize.x;
    depthAttachmentDesc.height = sceneData.viewportSize.y;
    const auto depthAttachment = Device::Get()->CreateTexture(depthAttachmentDesc, {});

    BufferDescriptor bufferDesc{};
    bufferDesc.type = BufferType::UNIFORM;
    bufferDesc.size = sizeof(MaterialData);
    bufferDesc.memoryType = MemoryType::CPU_GPU;
    sceneData.materialUniformBuffer = Device::Get()->CreateBuffer(bufferDesc, nullptr);

    bufferDesc.size = sizeof(LightBuffer);
    sceneData.lightUniformBuffer = Device::Get()->CreateBuffer(bufferDesc, nullptr);

    bufferDesc.size = sizeof(TransformBuffer);
    sceneData.transBuffer = Device::Get()->CreateBuffer(bufferDesc, nullptr);

    bufferDesc.size = sizeof(VPBuffer);
    sceneData.vpBuffer = Device::Get()->CreateBuffer(bufferDesc, nullptr);

    bufferDesc.size = sizeof(CameraPosBuffer);
    sceneData.cameraPosBuffer = Device::Get()->CreateBuffer(bufferDesc, nullptr);

    bufferDesc.size = sizeof(skyboxVertices);
    bufferDesc.type = BufferType::VERTEX;
    bufferDesc.memoryType = MemoryType::CPU_GPU;
    sceneData.skyboxVertexBuffer = Device::Get()->CreateBuffer(bufferDesc, &skyboxVertices);

    GraphicsPipelineDescriptor pipelineDescriptor;
    pipelineDescriptor.shader = shader;

    RenderPassDescriptor renderPassDescriptor{};
    renderPassDescriptor.extent = {sceneData.viewportSize.x, sceneData.viewportSize.y};
    renderPassDescriptor.offscreen = true;
    AttachmentDescriptor depth{};
    depth.loadOperation = AttachmentLoadOperation::CLEAR;
    depth.texture = depthAttachment;
    AttachmentDescriptor color{};
    color.texture = colorAttachment;
    color.loadOperation = AttachmentLoadOperation::CLEAR;
    color.storeOperation = AttachmentStoreOperation::STORE;
    AttachmentDescriptor normal{};
    normal.loadOperation = AttachmentLoadOperation::CLEAR;
    normal.texture = normalAttachemnt;
    renderPassDescriptor.colorAttachments = {color};
    renderPassDescriptor.depthStencilAttachment = {depth};
    sceneData.pbrPipeline = Device::Get()->CreateGraphicsPipeline(pipelineDescriptor, renderPassDescriptor);
    sceneData.pbrPipelineState = RendererCommand::CreateRendererState();

    sceneData.environmentHandle = assetManager.GetLoader<EnvironmentMapLoader>()->Load("/Assets/Textures/env_helipad.hdr");

    sceneData.pbrPipelineState->SetVertexBuffer(sceneData.vpBuffer, 0);
    sceneData.pbrPipelineState->SetVertexBuffer(sceneData.transBuffer, 1);
    sceneData.pbrPipelineState->SetFragmentBuffer(sceneData.materialUniformBuffer, 2);
    sceneData.pbrPipelineState->SetTexture(assetManager.GetAsset<Texture>(textureData.albedo), 3);
    sceneData.pbrPipelineState->SetTexture(assetManager.GetAsset<Texture>(textureData.normal), 4);
    sceneData.pbrPipelineState->SetTexture(assetManager.GetAsset<Texture>(textureData.metallic), 5);
    sceneData.pbrPipelineState->SetTexture(assetManager.GetAsset<Texture>(textureData.roughness), 6);
    sceneData.pbrPipelineState->SetTexture(assetManager.GetAsset<Texture>(textureData.ao), 7);
    sceneData.pbrPipelineState->SetTexture(assetManager.GetAsset<EnvironmentContext>(sceneData.environmentHandle)->irradianceMap, 8);
    sceneData.pbrPipelineState->SetTexture(assetManager.GetAsset<EnvironmentContext>(sceneData.environmentHandle)->prefilterMap, 9);
    sceneData.pbrPipelineState->SetTexture(assetManager.GetAsset<EnvironmentContext>(sceneData.environmentHandle)->brdfLut, 10);
    sceneData.pbrPipelineState->SetFragmentBuffer(sceneData.lightUniformBuffer, 11);
    sceneData.pbrPipelineState->SetFragmentBuffer(sceneData.cameraPosBuffer, 12);

    sceneData.pbrPipelineState->GetTexture(3)->SetSampler(sampler);
    sceneData.pbrPipelineState->GetTexture(4)->SetSampler(sampler);
    sceneData.pbrPipelineState->GetTexture(5)->SetSampler(sampler);
    sceneData.pbrPipelineState->GetTexture(6)->SetSampler(sampler);
    sceneData.pbrPipelineState->GetTexture(7)->SetSampler(sampler);

    ShaderProgramDescriptor skyboxShaderDesc{};
    vertexShader.path = "/Assets/Shaders/skybox.vert";
    vertexShader.type = ShaderType::VERTEX;
    fragmentShader.path = "/Assets/Shaders/skybox.frag";
    fragmentShader.type = ShaderType::FRAGMENT;
    skyboxShaderDesc.shaders = {vertexShader, fragmentShader};
    VertexBufferLayout skyboxLayout;
    skyboxLayout.Push<glm::vec3>();
    skyboxShaderDesc.layout = skyboxLayout;
    skyboxShaderDesc.reflection.textures = {1};
    skyboxShaderDesc.reflection.buffers[{0, ShaderType::VERTEX}] = BufferType::UNIFORM;
    shader = Device::Get()->CreateShader(skyboxShaderDesc);

    pipelineDescriptor.shader = shader;
    pipelineDescriptor.depthCompareOp = CompareOp::LESS_OR_EQUAL;
    pipelineDescriptor.cullMode = CullMode::FRONT;
    renderPassDescriptor.extent = {sceneData.viewportSize.x, sceneData.viewportSize.y};
    renderPassDescriptor.offscreen = true;
    depth.loadOperation = AttachmentLoadOperation::LOAD;
    depth.texture = depthAttachment;
    color.loadOperation = AttachmentLoadOperation::LOAD;
    color.storeOperation = AttachmentStoreOperation::STORE;
    color.texture = colorAttachment;
    renderPassDescriptor.colorAttachments = {color};
    renderPassDescriptor.depthStencilAttachment = {depth};
    sceneData.skyboxPipeline = Device::Get()->CreateGraphicsPipeline(pipelineDescriptor, renderPassDescriptor);

    sceneData.skyboxCube = scene->CreateEntity();
    MeshComponent skyboxMesh;
    bufferDesc.size = sizeof(skyboxVertices);
    bufferDesc.memoryType = MemoryType::CPU_GPU;
    bufferDesc.type = BufferType::VERTEX;
    skyboxMesh.SetVisibility(false);
    skyboxMesh.SetVertexBuffer(Device::Get()->CreateBuffer(bufferDesc, nullptr), std::make_shared<VertexBufferLayout>(skyboxLayout));
    sceneData.skyboxCube->AddComponent<MeshComponent>(skyboxMesh);
    sceneData.skyboxCube->AddComponent<TagComponent>(TagComponent("Skybox", sceneData.newEntityId++));
    auto& skyboxComponent = sceneData.skyboxCube->AddComponent<SkyboxComponent>();
    skyboxComponent.environment = assetManager.GetAsset<EnvironmentContext>(sceneData.environmentHandle);
    sceneData.skyboxPipelineState = RendererCommand::CreateRendererState();
    sceneData.skyboxPipelineState->SetVertexBuffer(sceneData.vpBuffer, 0);
    sceneData.skyboxPipelineState->SetTexture(skyboxComponent.environment->envMap, 1);
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

    TextureDescriptor presentAttachmentDesc{};
    presentAttachmentDesc.format = Format::BGRA8_SRGB;
    presentAttachmentDesc.type = TextureType::TEXTURE_2D;
    presentAttachmentDesc.width = windowW;
    presentAttachmentDesc.height = windowH;
    const auto presentAttachment = Device::Get()->CreateTexture(presentAttachmentDesc, {});
    presentAttachment->SetSampler(sampler);

    GraphicsPipelineDescriptor presentPipelineDesc{};
    presentPipelineDesc.shader = nullptr;
    RenderPassDescriptor presentRenderPassDescriptor{};
    presentRenderPassDescriptor.extent = { windowW, windowH };
    presentRenderPassDescriptor.offscreen = false;
    AttachmentDescriptor presentColor{};
    presentColor.texture = presentAttachment;
    presentColor.loadOperation = AttachmentLoadOperation::LOAD;
    presentRenderPassDescriptor.colorAttachments = { presentColor };

    sceneData.presentPipeline = Device::Get()->CreateGraphicsPipeline(presentPipelineDesc, presentRenderPassDescriptor);
    EventDispatcher::Get().Subscribe(MouseMovedEvent::descriptor, EVENT_CALLBACK(SandboxLayer::OnEvent));
    GraphicsPipelineDescriptor uiPipelineDesc{};
    uiPipelineDesc.shader = nullptr;

    AttachmentDescriptor uiAttachment{};
    uiAttachment.texture = presentAttachment;
    uiAttachment.loadOperation = AttachmentLoadOperation::CLEAR;
    uiAttachment.storeOperation = AttachmentStoreOperation::STORE;

    RenderPassDescriptor uiRenderpass{};
    uiRenderpass.extent = { windowW, windowH };
    uiRenderpass.offscreen = true;
    uiRenderpass.colorAttachments.emplace_back(uiAttachment);

    sceneData.uiPipeline = Device::Get()->CreateGraphicsPipeline(uiPipelineDesc, uiRenderpass);

    sceneData.imGuiLayer = std::make_shared<ImGuiLayer>(sceneData.uiPipeline);
    Application::Get().PushOverlay(sceneData.imGuiLayer);
}

void SandboxLayer::OnUpdate(float ts)
{
    if (sceneData.newViewportSize.x != 0 && sceneData.newViewportSize.y != 0)
    {
        sceneData.pbrPipeline->Resize(static_cast<uint32_t>(sceneData.newViewportSize.x), static_cast<uint32_t>(sceneData.newViewportSize.y));
        GraphicsPipelineDescriptor pipelineDescriptor;
        pipelineDescriptor.shader = sceneData.skyboxPipeline->GetPipelineDescriptor().shader;
        pipelineDescriptor.depthCompareOp = CompareOp::LESS_OR_EQUAL;
        pipelineDescriptor.cullMode = CullMode::FRONT;

        RenderPassDescriptor renderPassDescriptor;
        renderPassDescriptor.extent = {sceneData.viewportSize.x, sceneData.viewportSize.y};
        renderPassDescriptor.offscreen = true;
        AttachmentDescriptor depth;
        depth.loadOperation = AttachmentLoadOperation::LOAD;
        depth.texture = sceneData.pbrPipeline->GetRenderPassDescriptor().depthStencilAttachment.texture;
        AttachmentDescriptor color;
        color.loadOperation = AttachmentLoadOperation::LOAD;
        color.storeOperation = AttachmentStoreOperation::STORE;
        color.texture = sceneData.pbrPipeline->GetRenderPassDescriptor().colorAttachments.front().texture;
        renderPassDescriptor.colorAttachments = {color};
        renderPassDescriptor.depthStencilAttachment = {depth};

        sceneData.skyboxPipelineState = RendererCommand::CreateRendererState();
        sceneData.skyboxPipelineState->SetVertexBuffer(sceneData.vpBuffer, 0);
        sceneData.skyboxPipelineState->SetTexture(AssetManager::Get().GetAsset<EnvironmentContext>(sceneData.environmentHandle)->envMap, 1);

        sceneData.skyboxPipeline = Device::Get()->CreateGraphicsPipeline(pipelineDescriptor, renderPassDescriptor);
        sceneData.newViewportSize = {0, 0};
    }

    if (Input::IsKeyDown(R_KEY_C))
    {
        sceneData.camera->SetActive(!sceneData.camera->IsActive());
    }

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

    scene->OnUpdate(ts);

    renderer->SetPipeline(sceneData.pbrPipeline);
    renderer->BeginFrame(nullptr);
    sceneData.pbrPipelineState->OnUpdate(sceneData.pbrPipeline);
    renderer->EncodeState(sceneData.pbrPipelineState);
    VPBuffer buffer;
    auto projection = sceneData.camera->GetProjectionMatrix();
    projection[1][1] *= -1;
    buffer.viewProjection = projection * sceneData.camera->GetViewMatrix();
    auto ptr = sceneData.vpBuffer->Map();
    memcpy(ptr, &buffer, sizeof(VPBuffer));
    sceneData.vpBuffer->UnMap();

    LightBuffer lightBuffer;
    std::memset(&lightBuffer, 0, sizeof(LightBuffer));
    for (const auto& entityID: scene->GetRegistry().view<LightComponent>())
    {
        const auto& transform = scene->GetRegistry().get<TransformComponent>(entityID);
        const auto& light = scene->GetRegistry().get<LightComponent>(entityID);
        ShaderLight shaderLight;
        shaderLight.type = static_cast<int>(light.type);
        shaderLight.position = glm::vec4(transform.GetWorldPosition(), 1);
        shaderLight.color = glm::vec4(light.color, 1);
        shaderLight.intensity = light.intensity;
        lightBuffer.light[lightBuffer.lightsAmount.x] = shaderLight;
        lightBuffer.lightsAmount.x += 1;
        switch (light.type)
        {
            case LightType::DIRECTIONAL:
                lightBuffer.lightsAmount.y += 1;
                break;
            default:
            R_ASSERT(false, "");
        }
    }

    ptr = sceneData.lightUniformBuffer->Map();
    memcpy(ptr, &lightBuffer, sizeof(lightBuffer));
    sceneData.lightUniformBuffer->UnMap();

    R_ASSERT(lightBuffer.lightsAmount.x < 30, "");

    CameraPosBuffer cameraPosBuffer;
    cameraPosBuffer.pos = glm::vec4(sceneData.camera->GetPosition(), 0);
    ptr = sceneData.cameraPosBuffer->Map();
    memcpy(ptr, &cameraPosBuffer, sizeof(CameraPosBuffer));
    sceneData.cameraPosBuffer->UnMap();

    for (const auto& entity: scene->GetRegistry().view<MeshComponent>())
    {
        const auto& transform = scene->GetRegistry().get<TransformComponent>(entity);
        const auto& mesh = scene->GetRegistry().get<MeshComponent>(entity);
        if (!mesh.IsVisible())
        {
            continue;
        }

        auto& materialData = mesh.GetMaterial()->materialData;
        const auto& textureData = mesh.GetMaterial()->textureData;
        TransformBuffer transformBuffer;
        transformBuffer.transform = transform.GetWorldTransformMatrix();
        ptr = sceneData.transBuffer->Map();
        memcpy(ptr, &transformBuffer, sizeof(transformBuffer));
        sceneData.transBuffer->UnMap();

        const auto& material = mesh.GetMaterial();
        BindTextures(material->materialData, material->textureData);
        ptr = sceneData.materialUniformBuffer->Map();
        memcpy(ptr, &material->materialData, sizeof(MaterialData));
        sceneData.materialUniformBuffer->UnMap();
        renderer->Draw(mesh.GetVertexBuffer(), mesh.GetIndexBuffer());
    }

    renderer->EndFrame();

    projection = sceneData.camera->GetProjectionMatrix();
    projection[1][1] *= -1;
    auto cameraTransform = TransformComponent();
    cameraTransform.SetPosition(sceneData.camera->GetPosition());
    cameraTransform.RecalculateTransform();
    buffer.viewProjection = projection * glm::mat4(glm::mat3(scene->GetCamera()->GetViewMatrix()));
    ptr = sceneData.vpBuffer->Map();
    memcpy(ptr, &buffer, sizeof(VPBuffer));
    sceneData.vpBuffer->UnMap();

    renderer->SetPipeline(sceneData.skyboxPipeline);
    renderer->BeginFrame(nullptr);
    sceneData.skyboxPipelineState->OnUpdate(sceneData.skyboxPipeline);
    renderer->EncodeState(sceneData.skyboxPipelineState);
    renderer->Draw(sceneData.skyboxVertexBuffer);
    renderer->EndFrame();

    renderer->SetPipeline(sceneData.uiPipeline);
    renderer->BeginFrame(nullptr);
    sceneData.imGuiLayer->Begin();
    OnImGuiRender();
    sceneData.imGuiLayer->End(renderer->GetCmd());
    renderer->EndFrame();

    renderer->SetPipeline(sceneData.presentPipeline);
    renderer->BeginFrame(nullptr);
    renderer->EndFrame();

    const auto skyboxView = scene->GetRegistry().view<SkyboxComponent>();
    // TODO: Add black skybox for fallback
    R_ASSERT(!skyboxView.empty(), "No skybox was set!");
    R_ASSERT(skyboxView.size() == 1, "There must only 1 skybox in scene!");
    const auto& skyboxEntityID = skyboxView.front();
    const auto& skybox = scene->GetRegistry().get<SkyboxComponent>(skyboxEntityID);
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

            std::array<const char*, 3> aspectRatios = {"16/9", "4/3", "Fit to window"};
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
    const auto attachment = sceneData.pbrPipeline->GetRenderPassDescriptor().colorAttachments.front().texture;
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
