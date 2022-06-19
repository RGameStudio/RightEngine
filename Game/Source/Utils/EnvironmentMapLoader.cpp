#include "EnvironmentMapLoader.hpp"
#include "TextureLoader.hpp"
#include "MeshBuilder.hpp"
#include "Shader.hpp"
#include "FrameBuffer.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "RendererCommand.hpp"
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
    Mesh* cube;
    const auto projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const uint32_t maxMipLevels = 5;
}

EnvironmentMapLoader::EnvironmentMapLoader()
{
    cube = MeshBuilder::CubeGeometry();
}

EnvironmentMapLoader::~EnvironmentMapLoader()
{
    delete cube;
}

void EnvironmentMapLoader::Load(const std::string& path, bool flipVertically)
{
    loaderContext.path = path;
    loaderContext.flipVertically = flipVertically;
    ComputeEnvironmentMap();
    ComputeIrradianceMap();
    ComputeRadianceMap();
}

void EnvironmentMapLoader::ComputeEnvironmentMap()
{
    TextureLoader textureLoader;
    auto [data, textureSpec] = textureLoader.Load(loaderContext.path, loaderContext.flipVertically);
    loaderContext.specification = textureSpec;
    const auto equirectMap = Texture::Create(textureSpec, data);


    //TODO: Cache shader in shader library
    const auto envmapConverterShader = Shader::Create("/Assets/Shaders/Utils/envmap_to_cubemap.vert",
                                                      "/Assets/Shaders/Utils/envmap_to_cubemap.frag");

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
    auto cubemap = Texture3D::Create(textureSpec, {});
    fb.Bind();
    envmapConverterShader->Bind();
    equirectMap->Bind();
    const auto& va = cube->GetVertexArray();
    va->Bind();
    va->GetVertexBuffer()->Bind();
    for (int i = 0; i < 6; i++)
    {
        envmapConverterShader->SetUniformMat4f("u_ViewProjection", projectionMatrix * captureViews[i]);
        fb.BindAttachmentToTexture3DFace(cubemap, 0, i);
        RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RendererCommand::Draw(va->GetVertexBuffer());
    }
    fb.UnBind();

    environmentContext.envMap = cubemap;
    R_CORE_TRACE("Finished computing environment map for texture \"{0}\"", loaderContext.path);
}

void EnvironmentMapLoader::ComputeIrradianceMap()
{
    TextureSpecification texSpec = loaderContext.specification;
    const auto irradianceMapShader = Shader::Create("/Assets/Shaders/Utils/envmap_to_irr_map.vert",
                                                    "/Assets/Shaders/Utils/envmap_to_irr_map.frag");

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
    auto irradianceMap = Texture3D::Create(texSpec, {});
    fb.Bind();
    irradianceMapShader->Bind();
    environmentContext.envMap->Bind();
    const auto& va = cube->GetVertexArray();
    va->Bind();
    va->GetVertexBuffer()->Bind();
    for (int i = 0; i < 6; i++)
    {
        irradianceMapShader->SetUniformMat4f("u_ViewProjection", projectionMatrix * captureViews[i]);
        fb.BindAttachmentToTexture3DFace(irradianceMap, 0, i);
        RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RendererCommand::Draw(va->GetVertexBuffer());
    }
    fb.UnBind();

    environmentContext.irradianceMap = irradianceMap;
    R_CORE_TRACE("Finished computing irradiance map for texture \"{0}\"", loaderContext.path);
}

void EnvironmentMapLoader::ComputeRadianceMap()
{
    TextureSpecification prefilterTextureSpec{ prefilterTexWidth, prefilterTexHeight, 3, TextureFormat::RGB32F };
    const auto prefilterMapShader = Shader::Create("/Assets/Shaders/Utils/envmap_to_radiance_map.vert",
                                                   "/Assets/Shaders/Utils/envmap_to_radiance_map.frag");
    auto prefilteredMap = Texture3D::Create(prefilterTextureSpec, {});
    prefilteredMap->SetSampler(Sampler::Create({
                                                       SamplerFilter::Linear,
                                                       SamplerFilter::Linear,
                                                       SamplerFilter::Linear,
                                                       false }));
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
    prefilteredMap->GetSampler()->Bind();
    for (int mipLevel = 0; mipLevel < maxMipLevels; mipLevel++)
    {
        uint32_t mipWidth = prefilterTexWidth * std::pow(0.5, mipLevel);
        uint32_t mipHeight = prefilterTexHeight * std::pow(0.5, mipLevel);
        fb.Resize(mipWidth, mipHeight);
        fb.Bind();
        prefilterMapShader->Bind();
        prefilteredMap->Bind();
        environmentContext.envMap->Bind();
        const auto& va = cube->GetVertexArray();
        va->Bind();
        va->GetVertexBuffer()->Bind();

        float roughness = (float) mipLevel / (float) (maxMipLevels - 1);
        prefilterMapShader->SetUniform1f("u_Roughness", roughness);
        for (int i = 0; i < 6; i++)
        {
            prefilterMapShader->SetUniformMat4f("u_ViewProjection", projectionMatrix * captureViews[i]);
            fb.BindAttachmentToTexture3DFace(prefilteredMap, 0, i, mipLevel);
            RendererCommand::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            RendererCommand::Draw(va->GetVertexBuffer());
        }
    }
    fb.UnBind();

    environmentContext.prefilterMap = prefilteredMap;
    R_CORE_TRACE("Finished computing prefilter map for texture \"{0}\"", loaderContext.path);
}
