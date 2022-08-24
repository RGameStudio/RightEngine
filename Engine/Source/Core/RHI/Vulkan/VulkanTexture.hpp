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

    protected:
        void Init(const std::shared_ptr<VulkanDevice>& device,
             const std::vector<uint8_t>& data);

        std::shared_ptr<Buffer> stagingBuffer;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        std::shared_ptr<VulkanCommandBuffer> commandBuffer;
    };
}