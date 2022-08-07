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
}

EnvironmentMapLoader::EnvironmentMapLoader()
{
    cube = MeshBuilder::CubeGeometry();
}

void EnvironmentMapLoader::Load(const std::string& path, bool flipVertically)
{
    environmentContext = std::make_shared<EnvironmentContext>();
    loaderContext.path = path;
    loaderContext.flipVertically = flipVertically;
    environmentContext->name = GetTextureName(path);
    ComputeEnvironmentMap();
    ComputeIrradianceMap();
    ComputeRadianceMap();
    ComputeLUT();
}

void EnvironmentMapLoader::ComputeEnvironmentMap()
{
    TextureLoader textureLoader;
    auto [data, textureSpec] = textureLoader.Load(loaderContext.path, loaderContext.flipVertically);
    loaderContext.specification = textureSpec;
    textureSpec.type = TextureType::TEXTURE_2D;
    const auto equirectMap = Texture::Create(textureSpec, data);

    const auto envmapConverterShader = AssetManager::Get().LoadAsset<Shader>(
            "/Assets/Shaders/Utils/envmap_to_cubemap",
            "envmap_to_cubemap",
            LoaderOptions());

    FramebufferSpecification fbSpec;
    fbSpec.width = envTexWidth;
    fbSpec.height = envTexHeight;
    fbSpec.attachments = FramebufferAttachmentSpecification(
            {
                    FramebufferTextureSpecification(FramebufferTextureFormat::RGBA8)
            }
    );
    Framebuffer fb(fbSpec);

    textureSpec.width = envTexWidth;
    textureSpec.height = envTexHeight;
    textureSpec.type = TextureType::CUBEMAP;
    auto cubemap = Texture::Create(textureSpec, CubemapFaces());
    fb.Bind();
    envmapConverterShader->Bind();
    equirectMap->Bind();
    const auto& va = cube->GetVertexArray();
    va->Bind();
    va->GetVertexBuffer()->Bind();
    for (int i = 0; i < 6; i++)
    {
        envmapConverterShader->SetUniformMat4f("u_ViewProjection", projectionMatrix * captureViews[i]);
        fb.BindAttachmentToCubemapFace(cubemap, 0, i);
        RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        RendererCommand::Draw(va->GetVertexBuffer());
    }
    fb.UnBind();

    environmentContext->envMap = cubemap;
    environmentContext->equirectangularTexture = equirectMap;
    R_CORE_TRACE("Finished computing environment map for texture \"{0}\"", loaderContext.path);
}

void EnvironmentMapLoader::ComputeIrradianceMap()
{
    TextureSpecification texSpec = loaderContext.specification;
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
    TextureSpecification prefilterTextureSpec{ prefilterTexWidth,
                                               prefilterTexHeight,
                                               3,
                                               TextureType::CUBEMAP,
                                               TextureFormat::RGB32F };
    const auto prefilterMapShader = AssetManager::Get().LoadAsset<Shader>(
            "/Assets/Shaders/Utils/envmap_to_radiance_map",
            "envmap_to_radiance_map",
            LoaderOptions());
    auto prefilteredMap = Texture::Create(prefilterTextureSpec, CubemapFaces());
    prefilteredMap->SetSampler(Sampler::Create({
                                                       SamplerFilter::Linear,
                                                       SamplerFilter::Linear,
                                                       SamplerFilter::Linear,
                                                       true }));
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
    prefilteredMap->GetSampler()->Bind(1);
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
    TextureSpecification specification{ lutTexWidth,
                                        lutTexHeight,
                                        3,
                                        TextureType::TEXTURE_2D,
                                        TextureFormat::RGB32F };
    auto lutTexture = Texture::Create(specification, std::vector<uint8_t>());
    lutTexture->SetSampler(Sampler::Create({ SamplerFilter::Linear,
                                             SamplerFilter::Linear,
                                             SamplerFilter::Linear,
                                             false }));
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
    lutTexture->GetSampler()->Bind();
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
