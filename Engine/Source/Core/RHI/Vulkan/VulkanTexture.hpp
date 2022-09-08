#pragma once

#include "Texture.hpp"
#include "VulkanDevice.hpp"
#include "VulkanCommandBuffer.hpp"

namespace RightEngine
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(const std::shared_ptr<Device>& device,
                const TextureDescriptor& aSpecification,
                const std::vector<uint8_t>& data);

        virtual ~VulkanTexture() override;

        virtual void GenerateMipmaps() const override;
        virtual void Bind(uint32_t slot) const override;
        virtual void UnBind() const override;

        VkImageView GetImageView() const
        { return textureImageView; }

        VkImage GetImage() const
        { return textureImage; }

        static void ChangeImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    protected:
        void Init(const std::shared_ptr<VulkanDevice>& device,
             const std::vector<uint8_t>& data);

        std::shared_ptr<Buffer> stagingBuffer;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        std::shared_ptr<VulkanCommandBuffer> commandBuffer;
    };
}