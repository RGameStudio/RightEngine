#include "FrameBuffer.hpp"
#include "Assert.hpp"
#include <glad/glad.h>

using namespace RightEngine;

static const uint32_t maxFramebufferSize = 8192;

namespace Utils {

    static GLenum TextureTarget(bool multisampled)
    {
        return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
    {
        glCreateTextures(TextureTarget(multisampled), count, outID);
    }

    static void BindTexture(bool multisampled, uint32_t id)
    {
        glBindTexture(TextureTarget(multisampled), id);
    }

    static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
    }

    static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
    }

    static bool IsDepthFormat(FramebufferTextureFormat format)
    {
        switch (format)
        {
            case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
        }

        return false;
    }

    static GLenum FBTextureFormatToGL(FramebufferTextureFormat format)
    {
        switch (format)
        {
            case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
            case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
        }

        R_CORE_ASSERT(false, "");
        return 0;
    }

}

RightEngine::Framebuffer::Framebuffer(const RightEngine::FramebufferSpecification& spec): specification(spec)
{
    for (auto spec : specification.attachments.attachments)
    {
        if (!Utils::IsDepthFormat(spec.textureFormat))
        {
            colorAttachmentSpecifications.emplace_back(spec);
        }
        else
        {
            depthAttachmentSpecification = spec;
        }
    }

    Invalidate();
}

RightEngine::Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &id);
    glDeleteTextures(colorAttachments.size(), colorAttachments.data());
    glDeleteTextures(1, &depthAttachment);
}

void RightEngine::Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, specification.width, specification.height);
}

void RightEngine::Framebuffer::UnBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RightEngine::Framebuffer::Invalidate()
{
    if (id)
    {
        glDeleteFramebuffers(1, &id);
        glDeleteTextures(colorAttachments.size(), colorAttachments.data());
        glDeleteTextures(1, &depthAttachment);

        colorAttachments.clear();
        depthAttachment = 0;
    }

    glCreateFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    bool multisample = specification.samples > 1;

    // Attachments
    if (colorAttachmentSpecifications.size())
    {
        colorAttachments.resize(colorAttachmentSpecifications.size());
        Utils::CreateTextures(multisample, colorAttachments.data(), colorAttachments.size());

        for (size_t i = 0; i < colorAttachments.size(); i++)
        {
            Utils::BindTexture(multisample, colorAttachments[i]);
            switch (colorAttachmentSpecifications[i].textureFormat)
            {
                case FramebufferTextureFormat::RGBA8:
                    Utils::AttachColorTexture(colorAttachments[i], specification.samples, GL_RGBA8, GL_RGBA, specification.width, specification.height, i);
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    Utils::AttachColorTexture(colorAttachments[i], specification.samples, GL_R32I, GL_RED_INTEGER, specification.width, specification.height, i);
                    break;
            }
        }
    }

    if (depthAttachmentSpecification.textureFormat != FramebufferTextureFormat::None)
    {
        Utils::CreateTextures(multisample, &depthAttachment, 1);
        Utils::BindTexture(multisample, depthAttachment);
        switch (depthAttachmentSpecification.textureFormat)
        {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                Utils::AttachDepthTexture(depthAttachment, specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, specification.width, specification.height);
                break;
        }
    }

    if (colorAttachments.size() > 1)
    {
        R_CORE_ASSERT(colorAttachments.size() <= 4, "");
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(colorAttachments.size(), buffers);
    }
    else if (colorAttachments.empty())
    {
        // Only depth-pass
        glDrawBuffer(GL_NONE);
    }

    R_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RightEngine::Framebuffer::Resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0 || width > maxFramebufferSize || height > maxFramebufferSize)
    {
        R_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
        return;
    }
    specification.width = width;
    specification.height = height;

    Invalidate();
}

int RightEngine::Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
    R_CORE_ASSERT(attachmentIndex < colorAttachments.size(), "");

    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    int pixelData;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    return pixelData;
}

void Framebuffer::ReadPixels(uint32_t attachmentIndex, int x, int y, int width, int height, void* data)
{
    R_CORE_ASSERT(attachmentIndex < colorAttachments.size(), "");

    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

    auto spec = colorAttachmentSpecifications[attachmentIndex];
    if (spec.textureFormat == FramebufferTextureFormat::RGBA8)
    {
        glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        R_CORE_ASSERT(false, "Unsupported format!");
    }
}

void RightEngine::Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
    R_CORE_ASSERT(attachmentIndex < colorAttachments.size(), "");

    auto& spec = colorAttachmentSpecifications[attachmentIndex];
    glClearTexImage(colorAttachments[attachmentIndex], 0,
                    Utils::FBTextureFormatToGL(spec.textureFormat), GL_INT, &value);
}

uint32_t RightEngine::Framebuffer::GetColorAttachment(uint32_t index) const
{
    R_CORE_ASSERT(index < colorAttachments.size(), "");
    return colorAttachments[index];
}

const RightEngine::FramebufferSpecification& RightEngine::Framebuffer::GetSpecification() const
{
    return specification;
}
