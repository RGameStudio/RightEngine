#pragma once

#include <initializer_list>
#include <vector>
#include <memory>

namespace RightEngine
{
    class Texture3D;

    enum class FramebufferTextureFormat
    {
        None = 0,

        // Color
        RGBA8,
        RED_INTEGER,

        // Depth/stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };

    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;

        explicit FramebufferTextureSpecification(FramebufferTextureFormat format) : textureFormat(format)
        {}

        FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;

        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
                : attachments(attachments)
        {}

        std::vector<FramebufferTextureSpecification> attachments;
    };

    struct FramebufferSpecification
    {
        uint32_t width = 0, height = 0;
        FramebufferAttachmentSpecification attachments;
        uint32_t samples = 1;

        bool swapChainTarget = false;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpecification& spec);
        ~Framebuffer();

        void Bind();
        void UnBind();

        void Invalidate();

        void Resize(uint32_t width, uint32_t height);
        int ReadPixel(uint32_t attachmentIndex, int x, int y);
        void ReadPixels(uint32_t attachmentIndex, int x, int y, int width, int height, void* data);

        void ClearAttachment(uint32_t attachmentIndex, int value);
        uint32_t GetColorAttachment(uint32_t index = 0) const;

        const FramebufferSpecification& GetSpecification() const;

        void BindAttachmentToTexture3DFace(const std::shared_ptr<Texture3D>& texture3D,
                                           uint32_t attachmentIndex,
                                           uint32_t texture3DFace,
                                           uint32_t mipmapLevel = 0);

    private:
        uint32_t id;
        FramebufferSpecification specification;

        std::vector<FramebufferTextureSpecification> colorAttachmentSpecifications;
        FramebufferTextureSpecification depthAttachmentSpecification;

        std::vector<uint32_t> colorAttachments;
        uint32_t depthAttachment = 0;
    };
}