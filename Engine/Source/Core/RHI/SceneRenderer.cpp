#include "SceneRenderer.hpp"
#include "GraphicsPipeline.hpp"
#include "RendererState.hpp"
#include "RendererCommand.hpp"
#include "AssetManager.hpp"
#include "Application.hpp"

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

    std::shared_ptr<Texture> GetTexture(const AssetHandle& handle)
    {
        return AssetManager::Get().GetAsset<Texture>(handle);
    }
}

void SceneRenderer::Init()
{
    R_CORE_ASSERT(!isInitialized, "");
    isInitialized = true;

    defaultSampler = Device::Get()->CreateSampler({});
    const auto& window = Application::Get().GetWindow();
    windowSize = { window->GetWidth(), window->GetHeight() };

    CreateShaders();
    CreatePasses();
    CreateBuffers();
}

void SceneRenderer::CreateShaders()
{
    //PBR
    {
        ShaderProgramDescriptor shaderProgramDescriptor;
        ShaderDescriptor vertexShader;
        vertexShader.path = "/Assets/Shaders/pbr.vert";
        vertexShader.type = ShaderType::VERTEX;
        ShaderDescriptor fragmentShader;
        fragmentShader.path = "/Assets/Shaders/pbr.frag";
        fragmentShader.type = ShaderType::FRAGMENT;
        shaderProgramDescriptor.shaders = { vertexShader, fragmentShader };
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
        pbrShader = Device::Get()->CreateShader(shaderProgramDescriptor);
    }

    //Skybox
    {
        ShaderProgramDescriptor skyboxShaderDesc{};
        ShaderDescriptor vertexShader;
        vertexShader.path = "/Assets/Shaders/skybox.vert";
        vertexShader.type = ShaderType::VERTEX;
        ShaderDescriptor fragmentShader;
        fragmentShader.path = "/Assets/Shaders/skybox.frag";
        fragmentShader.type = ShaderType::FRAGMENT;
        skyboxShaderDesc.shaders = { vertexShader, fragmentShader };
        VertexBufferLayout skyboxLayout;
        skyboxLayout.Push<glm::vec3>();
        skyboxShaderDesc.layout = skyboxLayout;
        skyboxShaderDesc.reflection.textures = { 3 };
        skyboxShaderDesc.reflection.buffers[{ 1, ShaderType::VERTEX }] = BufferType::UNIFORM;
        skyboxShader = Device::Get()->CreateShader(skyboxShaderDesc);
    }
}

void SceneRenderer::CreatePasses()
{
    //PBR
    {
        TextureDescriptor colorAttachmentDesc{};
        colorAttachmentDesc.format = Format::BGRA8_SRGB;
        colorAttachmentDesc.type = TextureType::TEXTURE_2D;
        colorAttachmentDesc.width = viewport.x;
        colorAttachmentDesc.height = viewport.y;
        const auto colorAttachment = Device::Get()->CreateTexture(colorAttachmentDesc, {});
        colorAttachment->SetSampler(defaultSampler);
        TextureDescriptor normalAttachmentDesc{};
        normalAttachmentDesc.format = Format::RGBA16_SFLOAT;
        normalAttachmentDesc.type = TextureType::TEXTURE_2D;
        normalAttachmentDesc.width = viewport.x;
        normalAttachmentDesc.height = viewport.y;
        const auto normalAttachemnt = Device::Get()->CreateTexture(normalAttachmentDesc, {});
        TextureDescriptor depthAttachmentDesc{};
        depthAttachmentDesc.format = Format::D32_SFLOAT_S8_UINT;
        depthAttachmentDesc.type = TextureType::TEXTURE_2D;
        depthAttachmentDesc.width = viewport.x;
        depthAttachmentDesc.height = viewport.y;
        const auto depthAttachment = Device::Get()->CreateTexture(depthAttachmentDesc, {});

        GraphicsPipelineDescriptor pipelineDescriptor{};
        pipelineDescriptor.shader = pbrShader;

        RenderPassDescriptor renderPassDescriptor{};
        renderPassDescriptor.extent = viewport;
        renderPassDescriptor.offscreen = true;
        renderPassDescriptor.name = "PBR_main";
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
        renderPassDescriptor.colorAttachments = { color };
        renderPassDescriptor.depthStencilAttachment = { depth };
        pbrPipeline = Device::Get()->CreateGraphicsPipeline(pipelineDescriptor, renderPassDescriptor);
    }

    //Skybox
    {
        const auto& pbrPassDescriptor = pbrPipeline->GetRenderPassDescriptor();
        R_CORE_ASSERT(pbrPassDescriptor.colorAttachments.front().texture
                      && pbrPassDescriptor.depthStencilAttachment.texture, "");

        GraphicsPipelineDescriptor pipelineDescriptor;
        pipelineDescriptor.shader = skyboxShader;
        pipelineDescriptor.depthCompareOp = CompareOp::LESS_OR_EQUAL;
        pipelineDescriptor.cullMode = CullMode::FRONT;

        RenderPassDescriptor renderPassDescriptor{};
        renderPassDescriptor.extent = viewport;
        renderPassDescriptor.offscreen = true;

        AttachmentDescriptor depth{};
        depth.loadOperation = AttachmentLoadOperation::LOAD;
        depth.texture = pbrPassDescriptor.depthStencilAttachment.texture;

        AttachmentDescriptor color{};
        color.loadOperation = AttachmentLoadOperation::LOAD;
        color.storeOperation = AttachmentStoreOperation::STORE;
        color.texture = pbrPassDescriptor.colorAttachments.front().texture;

        renderPassDescriptor.colorAttachments = { color };
        renderPassDescriptor.depthStencilAttachment = { depth };
        skyboxPipeline = Device::Get()->CreateGraphicsPipeline(pipelineDescriptor, renderPassDescriptor);
    }

    // Present
    {
        TextureDescriptor presentAttachmentDesc{};
        presentAttachmentDesc.format = Format::BGRA8_SRGB;
        presentAttachmentDesc.type = TextureType::TEXTURE_2D;
        presentAttachmentDesc.width = windowSize.x;
        presentAttachmentDesc.height = windowSize.y;
        const auto presentAttachment = Device::Get()->CreateTexture(presentAttachmentDesc, {});
        presentAttachment->SetSampler(defaultSampler);

        GraphicsPipelineDescriptor presentPipelineDesc{};
        presentPipelineDesc.shader = nullptr;
        RenderPassDescriptor presentRenderPassDescriptor{};
        presentRenderPassDescriptor.extent = windowSize;
        presentRenderPassDescriptor.offscreen = false;
        AttachmentDescriptor presentColor{};
        presentColor.texture = presentAttachment;
        presentColor.loadOperation = AttachmentLoadOperation::LOAD;
        presentRenderPassDescriptor.colorAttachments = {presentColor};

        presentPipeline = Device::Get()->CreateGraphicsPipeline(presentPipelineDesc, presentRenderPassDescriptor);
    }

    // UI
    {
        const auto& presentPipelineAttachment = presentPipeline->GetRenderPassDescriptor().colorAttachments.front();

        AttachmentDescriptor uiAttachment{};
        uiAttachment.texture = presentPipelineAttachment.texture;
        uiAttachment.loadOperation = AttachmentLoadOperation::CLEAR;
        uiAttachment.storeOperation = AttachmentStoreOperation::STORE;

        RenderPassDescriptor uiRenderpass{};
        uiRenderpass.extent = windowSize;
        uiRenderpass.offscreen = true;
        uiRenderpass.colorAttachments.emplace_back(uiAttachment);

        GraphicsPipelineDescriptor uiPipelineDesc{};
        uiPipelineDesc.shader = nullptr;

        uiPipeline = Device::Get()->CreateGraphicsPipeline(uiPipelineDesc, uiRenderpass);
    }
}

void SceneRenderer::CreateBuffers()
{
    const size_t maxEntitiesAmount = 512;
    const size_t transformBufferSize = maxEntitiesAmount * sizeof(UBTransformData);
    const size_t materialDataBufferSize = maxEntitiesAmount * sizeof(MaterialData);
    drawList.reserve(maxEntitiesAmount);

    uniformBufferSet = std::make_shared<UniformBufferSet>(1);
    uniformBufferSet->Create(65536, 0);
    uniformBufferSet->Create(sizeof(UBCameraData), 1);
    uniformBufferSet->Create(65536, 2);
    uniformBufferSet->Create(sizeof(UBLightData), 11);

    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = sizeof(skyboxVertices);
    bufferDescriptor.type = BufferType::VERTEX;
    bufferDescriptor.memoryType = MemoryType::CPU_GPU;
    skyboxVertexBuffer = Device::Get()->CreateBuffer(bufferDescriptor, &skyboxVertices);
}

void SceneRenderer::SubmitMeshNode(const std::shared_ptr<MeshNode>& meshNode, const std::shared_ptr<Material>& material, const glm::mat4& transform)
{
    for (const auto& mesh : meshNode->meshes)
    {
        SubmitMesh(mesh, material, transform);
    }

    for (const auto& child : meshNode->children)
    {
        SubmitMeshNode(child, material, transform);
    }
}

void SceneRenderer::SubmitMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const glm::mat4& transform)
{
    DrawCommand dc;
    dc.mesh = mesh;
    dc.material = material;
    dc.transform = transform;

    drawList.emplace_back(dc);
}

void SceneRenderer::BeginScene(const std::shared_ptr<Camera>& camera,
                               const std::shared_ptr<EnvironmentContext>& environment,
                               const std::vector<LightData>& lights)
{
    R_CORE_ASSERT(lights.size() < 30, "");
    cameraDataUB.position = glm::vec4(camera->GetPosition(), 1.0);
    auto projection = camera->GetProjectionMatrix();
    projection[1][1] *= -1;
    cameraDataUB.viewProjection = projection * camera->GetViewMatrix();

    lightDataUB.lightsAmount = lights.size();
    memcpy(&lightDataUB.light, lights.data(), lights.size());
    sceneEnvironment = *environment;

    uniformBufferSet->Get(1)->SetData(&cameraDataUB, sizeof(cameraDataUB));
    uniformBufferSet->Get(11)->SetData(&lightDataUB, sizeof(lightDataUB));
}

void SceneRenderer::EndScene()
{
    PBRPass();
    SkyboxPass();

    UIPass();
    Present();
    Clear();
}

void SceneRenderer::PBRPass()
{
    renderer.SetPipeline(pbrPipeline);
    renderer.BeginFrame(nullptr);
    std::vector<std::shared_ptr<RendererState>> rendererStates(512);

    auto& transformBuffer = uniformBufferSet->Get(0);
    auto& cameraBuffer = uniformBufferSet->Get(1);
    auto& materialBuffer = uniformBufferSet->Get(2);
    auto& lightBuffer = uniformBufferSet->Get(11);

    uint32_t transformBufferOffset = 0;
    uint32_t materialBufferOffset = 0;
    for (int i = 0; i < drawList.size(); i++)
    {
        auto& dc = drawList[i];
        const size_t transformDataSize = Device::Get()->GetAlignedGPUDataSize(sizeof(UBTransformData));
        const size_t materialDataSize = Device::Get()->GetAlignedGPUDataSize(sizeof(MaterialData));
        transformBuffer->SetData(&dc.transform, transformDataSize, transformBufferOffset);
        materialBuffer->SetData(&dc.material->materialData, materialDataSize, materialBufferOffset);

        auto& rs = rendererStates[i];
        rs = RendererCommand::CreateRendererState();
        rs->SetVertexBuffer(transformBuffer, 0, transformBufferOffset, sizeof(UBTransformData));
        rs->SetVertexBuffer(cameraBuffer, 1);
        rs->SetFragmentBuffer(materialBuffer, 2, materialBufferOffset, sizeof(MaterialData));
        rs->SetFragmentBuffer(lightBuffer, 11);

        rs->SetTexture(GetTexture(dc.material->textureData.albedo), 3);
        rs->SetTexture(GetTexture(dc.material->textureData.normal), 4);
        rs->SetTexture(GetTexture(dc.material->textureData.metallic), 5);
        rs->SetTexture(GetTexture(dc.material->textureData.roughness), 6);
        rs->SetTexture(GetTexture(dc.material->textureData.ao), 7);
        rs->SetTexture(sceneEnvironment.irradianceMap, 8);
        rs->SetTexture(sceneEnvironment.prefilterMap, 9);
        rs->SetTexture(sceneEnvironment.brdfLut, 10);

        rs->OnUpdate(renderer.GetActivePipeline());
        renderer.EncodeState(rs);
        renderer.Draw(dc.mesh);

        transformBufferOffset += transformDataSize;
        materialBufferOffset += materialDataSize;
    }
    renderer.EndFrame();
}

void SceneRenderer::SkyboxPass()
{
    renderer.SetPipeline(skyboxPipeline);
    renderer.BeginFrame(nullptr);

    auto& cameraBuffer = uniformBufferSet->Get(1);
    const auto& camera = scene->GetCamera();

    UBCameraData skyboxCameraData{};
    auto projection = camera->GetProjectionMatrix();
    projection[1][1] *= -1;
    skyboxCameraData.viewProjection = projection * glm::mat4(glm::mat3(camera->GetViewMatrix()));
    cameraBuffer->SetData(&skyboxCameraData, sizeof(UBCameraData));

    auto rs = RendererCommand::CreateRendererState();
    rs->SetVertexBuffer(cameraBuffer, 1);
    rs->SetTexture(sceneEnvironment.envMap, 3);
    rs->OnUpdate(renderer.GetActivePipeline());

    renderer.EncodeState(rs);
    renderer.Draw(skyboxVertexBuffer);
    renderer.EndFrame();
}

void SceneRenderer::UIPass()
{
    renderer.SetPipeline(uiPipeline);
    renderer.BeginFrame(nullptr);
    R_CORE_ASSERT(uiPassCallback, "");
    uiPassCallback(renderer.GetCmd());
    renderer.EndFrame();
}

void SceneRenderer::Present()
{
    renderer.SetPipeline(presentPipeline);
    renderer.BeginFrame(nullptr);
    renderer.EndFrame();
}

void SceneRenderer::Clear()
{
    drawList.clear();
}

void SceneRenderer::Resize(int x, int y)
{
    pbrPipeline->Resize(x, y);
    skyboxPipeline->Resize(x, y);
}

const std::shared_ptr<GraphicsPipeline>& SceneRenderer::GetPass(PassType type) const
{
    switch (type)
    {
        case PassType::UI:
            return uiPipeline;
        case PassType::PBR:
            return pbrPipeline;
        case PassType::SKYBOX:
            return skyboxPipeline;
        case PassType::PRESENT:
            return presentPipeline;
        default:
            R_CORE_ASSERT(false, "")
    }
}
