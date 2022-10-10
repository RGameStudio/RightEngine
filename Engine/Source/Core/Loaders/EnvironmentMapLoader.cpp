#include "EnvironmentMapLoader.hpp"
#include "TextureLoader.hpp"
#include "MeshBuilder.hpp"
#include "Shader.hpp"
#include "FrameBuffer.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "RendererCommand.hpp"
#include "String.hpp"
#include "AssetManager.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

using namespace RightEngine;

namespace
{
    const uint32_t envTexWidth = 2048;
    const uint32_t envTexHeight = 2048;
    const uint32_t irradianceTexWidth = 64;
    const uint32_t irradianceTexHeight = 64;
    const uint32_t prefilterTexWidth = 128;
    const uint32_t prefilterTexHeight = 128;
    const uint32_t lutTexWidth = 512;
    const uint32_t lutTexHeight = 512;

    const glm::mat4 captureViews[] =
            {
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(1.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, -1.0f, 0.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(-1.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, -1.0f, 0.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, -1.0f, 0.0f),
                                glm::vec3(0.0f, 0.0f, -1.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f),
                                glm::vec3(0.0f, -1.0f, 0.0f)),
                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, 0.0f, -1.0f),
                                glm::vec3(0.0f, -1.0f, 0.0f))
            };
    std::shared_ptr<MeshComponent> cube;
    const auto projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const uint32_t maxMipLevels = 5;

    std::string GetTextureName(const std::string& path)
    {
        const auto splittedPath = String::Split(path, "/");
        return splittedPath.back();
    }

    const float cubeVertexData[] = {
            // [position 3] [normal 3] [texture coodinate 2]
            // back face
            -1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            // front face
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            // left face
            -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, 1.0f, -1.0f,  // top-left
            -1.0f, -1.0f, -1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f,   // bottom-left
            -1.0f, -1.0f, 1.0f,   // bottom-right
            -1.0f, 1.0f, 1.0f,  // top-right
            // right face
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            // bottom face
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            // top face
            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
    };
}

EnvironmentMapLoader::EnvironmentMapLoader()
{
//    cube = MeshBuilder::CubeGeometry();
}

void EnvironmentMapLoader::Load(const std::string& path, bool flipVertically)
{
    environmentContext = std::make_shared<EnvironmentContext>();
    loaderContext.path = path;
    loaderContext.flipVertically = flipVertically;
    environmentContext->name = GetTextureName(path);
    ComputeEnvironmentMap();
//    ComputeIrradianceMap();
//    ComputeRadianceMap();
//    ComputeLUT();
}

void EnvironmentMapLoader::ComputeEnvironmentMap()
{
    TextureLoader textureLoader;
    auto [data, textureSpec] = textureLoader.Load(loaderContext.path, loaderContext.flipVertically);
    textureSpec.format = Format::RGBA8_SRGB;
    loaderContext.specification = textureSpec;
    textureSpec.type = TextureType::TEXTURE_2D;
    const auto equirectMap = Device::Get()->CreateTexture(textureSpec, data);

    ShaderProgramDescriptor shaderProgramDescriptor;
    ShaderDescriptor vertexShader;
    vertexShader.path = "/Assets/Shaders/Utils/envmap_to_cubemap.vert";
    vertexShader.type = ShaderType::VERTEX;
    ShaderDescriptor fragmentShader;
    fragmentShader.path = "/Assets/Shaders/Utils/envmap_to_cubemap.frag";
    fragmentShader.type = ShaderType::FRAGMENT;
    shaderProgramDescriptor.shaders = { vertexShader, fragmentShader };
    VertexBufferLayout layout;
    layout.Push<glm::vec3>();
    shaderProgramDescriptor.layout = layout;
    shaderProgramDescriptor.reflection.textures = { 1 };
    shaderProgramDescriptor.reflection.buffers[{ 0, ShaderType::VERTEX }] = BUFFER_TYPE_UNIFORM;
    const auto shader = Device::Get()->CreateShader(shaderProgramDescriptor);

    TextureDescriptor cubeDesc;
    cubeDesc.type = TextureType::CUBEMAP;
    cubeDesc.width = envTexWidth;
    cubeDesc.height = envTexHeight;
    cubeDesc.format = Format::RGBA16_SFLOAT;
    cubeDesc.componentAmount = 3;
    const auto cubemap = Device::Get()->CreateTexture(cubeDesc, {});

    TextureDescriptor colorAttachmentDesc{};
    colorAttachmentDesc.format = Format::RGBA16_SFLOAT;
    colorAttachmentDesc.type = TextureType::TEXTURE_2D;
    colorAttachmentDesc.width = envTexWidth;
    colorAttachmentDesc.height = envTexHeight;
    const auto colorAttachment = Device::Get()->CreateTexture(colorAttachmentDesc, {});
    TextureDescriptor depthAttachmentDesc{};
    depthAttachmentDesc.format = Format::D32_SFLOAT_S8_UINT;
    depthAttachmentDesc.type = TextureType::TEXTURE_2D;
    depthAttachmentDesc.width = envTexWidth;
    depthAttachmentDesc.height = envTexHeight;
    const auto depthAttachment = Device::Get()->CreateTexture(depthAttachmentDesc, {});

    RenderPassDescriptor renderPassDescriptor{};
    renderPassDescriptor.extent = { envTexWidth, envTexHeight };
    renderPassDescriptor.offscreen = true;
    AttachmentDescriptor depth{};
    depth.loadOperation = AttachmentLoadOperation::CLEAR;
    depth.texture = depthAttachment;
    AttachmentDescriptor color{};
    color.texture = colorAttachment;
    color.loadOperation = AttachmentLoadOperation::CLEAR;
    renderPassDescriptor.colorAttachments = { color };
    renderPassDescriptor.depthStencilAttachment = { depth };

    GraphicsPipelineDescriptor pipelineDescriptor;
    pipelineDescriptor.shader = shader;

    const auto pipeline = Device::Get()->CreateGraphicsPipeline(pipelineDescriptor, renderPassDescriptor);

    Renderer renderer;
    renderer.SetPipeline(pipeline);

    const auto rendererState = RendererCommand::CreateRendererState();
    struct Camera
    {
        glm::mat4 view;
        glm::mat4 projection;
    };
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.type = BUFFER_TYPE_VERTEX;
    bufferDescriptor.size = sizeof(cubeVertexData);
    bufferDescriptor.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
    const auto vertexBuffer = Device::Get()->CreateBuffer(bufferDescriptor, cubeVertexData);

    bufferDescriptor.type = BUFFER_TYPE_UNIFORM;
    bufferDescriptor.size = sizeof(Camera);
    bufferDescriptor.memoryType = static_cast<MemoryType>(MEMORY_TYPE_HOST_COHERENT | MEMORY_TYPE_HOST_VISIBLE);
    const auto buffer = Device::Get()->CreateBuffer(bufferDescriptor, nullptr);
    rendererState->SetVertexBuffer(buffer, 0);
    rendererState->SetTexture(equirectMap, 1);

    SamplerDescriptor samplerDescriptor{};
    const auto sampler = Device::Get()->CreateSampler(samplerDescriptor);
    TextureDescriptor environmentCubemapDesc;
    environmentCubemapDesc.type = TextureType::CUBEMAP;
    environmentCubemapDesc.componentAmount = 4;
    environmentCubemapDesc.format = Format::RGBA16_SFLOAT;
    environmentCubemapDesc.height = envTexHeight;
    environmentCubemapDesc.width = envTexWidth;
    const auto environmentCubemap = Device::Get()->CreateTexture(environmentCubemapDesc, {});
    
    environmentCubemap->SetSampler(sampler);
    colorAttachment->SetSampler(sampler);
    equirectMap->SetSampler(sampler);
    
    TextureCopy src;
    src.usage = SHADER_READ_ONLY;
    src.layerNum = 0;
    src.mipLevel = 0;

    for (int i = 0; i < 6; i++)
    {
        Camera camera;
        camera.view = captureViews[i];
        camera.projection = projectionMatrix;
        void* ptr = buffer->Map();
        memcpy(ptr, &camera, sizeof(Camera));
        buffer->UnMap();
        rendererState->OnUpdate(pipeline);
        renderer.BeginFrame(nullptr);
        renderer.EncodeState(rendererState);
        renderer.Draw(vertexBuffer);
        renderer.EndFrame();
        
        TextureCopy dst;
        dst.usage = SHADER_READ_ONLY;
        dst.layerNum = i;
        dst.mipLevel = 0;
        environmentCubemap->CopyFrom(colorAttachment, src, dst);
    }
    
    environmentContext->envMap = environmentCubemap;
    environmentContext->equirectangularTexture = equirectMap;
}

void EnvironmentMapLoader::ComputeIrradianceMap()
{
    TextureDescriptor texSpec = loaderContext.specification;
    const auto irradianceMapShader = AssetManager::Get().LoadAsset<Shader>("/Assets/Shaders/Utils/envmap_to_irr_map",
                                                                           "envmap_to_irr_map", LoaderOptions());

    FramebufferSpecification fbSpec;
    fbSpec.width = irradianceTexWidth;
    fbSpec.height = irradianceTexHeight;
    fbSpec.attachments = FramebufferAttachmentSpecification(
            {
                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8)
            }
    );
    Framebuffer fb(fbSpec);

    texSpec.width = irradianceTexWidth;
    texSpec.height = irradianceTexHeight;
    texSpec.type = TextureType::CUBEMAP;
    auto irradianceMap = Texture::Create(texSpec, CubemapFaces());
    fb.Bind();
    irradianceMapShader->Bind();
    environmentContext->envMap->Bind();
    const auto& va = cube->GetVertexArray();
    va->Bind();
    va->GetVertexBuffer()->Bind();
    for (int i = 0; i < 6; i++)
    {
        irradianceMapShader->SetUniformMat4f("u_ViewProjection", projectionMatrix * captureViews[i]);
        fb.BindAttachmentToCubemapFace(irradianceMap, 0, i);
        RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        RendererCommand::Draw(va->GetVertexBuffer());
    }
    fb.UnBind();

    environmentContext->irradianceMap = irradianceMap;
    R_CORE_TRACE("Finished computing irradiance map for texture \"{0}\"", loaderContext.path);
}

void EnvironmentMapLoader::ComputeRadianceMap()
{
    R_CORE_ASSERT(false, "");
    TextureDescriptor prefilterTextureSpec{};
    const auto prefilterMapShader = AssetManager::Get().LoadAsset<Shader>(
            "/Assets/Shaders/Utils/envmap_to_radiance_map",
            "envmap_to_radiance_map",
            LoaderOptions());
    auto prefilteredMap = Texture::Create(prefilterTextureSpec, CubemapFaces());
//    prefilteredMap->SetSampler(Sampler::Create({
//                                                       SamplerFilter::Linear,
//                                                       SamplerFilter::Linear,
//                                                       SamplerFilter::Linear,
//                                                       true }));
    prefilteredMap->GenerateMipmaps();
    FramebufferSpecification fbSpec;
    fbSpec.width = prefilterTexWidth;
    fbSpec.height = prefilterTexHeight;
    fbSpec.attachments = FramebufferAttachmentSpecification(
            {
                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8)
            }
    );
    Framebuffer fb(fbSpec);
//    prefilteredMap->GetSampler()->Bind(1);
    for (int mipLevel = 0; mipLevel < maxMipLevels; mipLevel++)
    {
        uint32_t mipWidth = prefilterTexWidth * std::pow(0.5, mipLevel);
        uint32_t mipHeight = prefilterTexHeight * std::pow(0.5, mipLevel);
        fb.Resize(mipWidth, mipHeight);
        fb.Bind();
        prefilterMapShader->Bind();
        prefilteredMap->Bind(1);
        environmentContext->envMap->Bind();
        const auto& va = cube->GetVertexArray();
        va->Bind();
        va->GetVertexBuffer()->Bind();

        float roughness = (float) mipLevel / (float) (maxMipLevels - 1);
        prefilterMapShader->SetUniform1f("u_Roughness", roughness);
        for (int i = 0; i < 6; i++)
        {
            prefilterMapShader->SetUniformMat4f("u_ViewProjection", projectionMatrix * captureViews[i]);
            fb.BindAttachmentToCubemapFace(prefilteredMap, 0, i, mipLevel);
            RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            RendererCommand::Draw(va->GetVertexBuffer());
        }
    }
    fb.UnBind();

    environmentContext->prefilterMap = prefilteredMap;
    R_CORE_TRACE("Finished computing prefilter map for texture \"{0}\"", loaderContext.path);
}

void EnvironmentMapLoader::ComputeLUT()
{
    R_CORE_ASSERT(false, "");
    TextureDescriptor specification{};
    auto lutTexture = Texture::Create(specification, std::vector<uint8_t>());
//    lutTexture->SetSampler(Sampler::Create({ SamplerFilter::Linear,
//                                             SamplerFilter::Linear,
//                                             SamplerFilter::Linear,
//                                             false }));
    const auto lutShader = AssetManager::Get().LoadAsset<Shader>(
            "/Assets/Shaders/Utils/brdf",
            "brdf",
            LoaderOptions());

    FramebufferSpecification fbSpec;
    fbSpec.width = lutTexWidth;
    fbSpec.height = lutTexHeight;
    fbSpec.attachments = FramebufferAttachmentSpecification(
            {
                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8)
            }
    );
    Framebuffer fb(fbSpec);
    const auto quad = MeshBuilder::QuadGeometry();
    fb.Bind();
    lutShader->Bind();
//    lutTexture->GetSampler()->Bind();
    lutTexture->Bind();
    const auto& va = quad->GetVertexArray();
    va->Bind();
    va->GetVertexBuffer()->Bind();

    fb.BindAttachmentToTexture(lutTexture, 0);
    RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    RendererCommand::Draw(va->GetVertexBuffer());

    fb.UnBind();

    environmentContext->brdfLut = lutTexture;
}
