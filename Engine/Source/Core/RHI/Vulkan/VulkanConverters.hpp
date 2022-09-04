#pragma once

#include "Assert.hpp"
#include "Types.hpp"
#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>

namespace RightEngine
{
    class VulkanConverters
    {
    public:
        inline static VkPresentModeKHR PresentMode(PresentMode mode)
        {
            switch (mode)
            {
                case PresentMode::IMMEDIATE:
                    return VK_PRESENT_MODE_IMMEDIATE_KHR;
                case PresentMode::FIFO:
                    return VK_PRESENT_MODE_FIFO_KHR;
                case PresentMode::MAILBOX:
                    return VK_PRESENT_MODE_MAILBOX_KHR;
            }
        }

        inline static VkExtent2D Extent(const glm::ivec2& extent)
        {
            return { static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y) };
        }

        inline static VkFormat Format(Format format)
        {
            switch (format)
            {
                case Format::RGBA8_SRGB:
                    return VK_FORMAT_R8G8B8A8_SRGB;
                case Format::BGRA8_SRGB:
                    return VK_FORMAT_B8G8R8A8_SRGB;
                case Format::R32_SFLOAT:
                    return VK_FORMAT_R32_SFLOAT;
                case Format::RG32_SFLOAT:
                    return VK_FORMAT_R32G32_SFLOAT;
                case Format::RGB32_SFLOAT:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case Format::RGBA32_SFLOAT:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
                case Format::R32_UINT:
                    return VK_FORMAT_R32_UINT;
                case Format::R8_UINT:
                    return VK_FORMAT_R8_UINT;
                case Format::RGBA8_UINT:
                    return VK_FORMAT_R8G8B8_UINT;
                case Format::D24_UNORM_S8_UINT:
                    return VK_FORMAT_D24_UNORM_S8_UINT;
                default:
                    R_CORE_ASSERT(false, "");
            }
        }

        inline static VkBufferUsageFlags BufferUsage(BufferType type)
        {
            VkBufferUsageFlags flags = 0;
            if (type & BUFFER_TYPE_VERTEX)
            {
                flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            }
            if (type & BUFFER_TYPE_TRANSFER_SRC)
            {
                flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            }
            if (type & BUFFER_TYPE_TRANSFER_DST)
            {
                flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            }
            if (type & BUFFER_TYPE_INDEX)
            {
                flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            }
            if (type & BUFFER_TYPE_UNIFORM)
            {
                flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            }

            return flags;
        }

        inline static VkMemoryPropertyFlags MemoryProperty(MemoryType type)
        {
            VkMemoryPropertyFlags flags = 0;
            if (type & MEMORY_TYPE_DEVICE_LOCAL)
            {
                flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            }
            if (type & MEMORY_TYPE_HOST_VISIBLE)
            {
                flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            }
            if (type & MEMORY_TYPE_HOST_COHERENT)
            {
                flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            }

            return flags;
        }

        inline static VkShaderStageFlags ShaderStage(ShaderStage stage)
        {
            switch (stage)
            {
                case ShaderType::VERTEX:
                    return VK_SHADER_STAGE_VERTEX_BIT;
                case ShaderType::FRAGMENT:
                    return VK_SHADER_STAGE_FRAGMENT_BIT;
                default:
                R_CORE_ASSERT(false, "");
            }
        }

        inline static VkImageType ImageType(TextureType type)
        {
            switch (type)
            {
                case TextureType::CUBEMAP:
                    return VK_IMAGE_TYPE_3D;
                case TextureType::TEXTURE_2D:
                    return VK_IMAGE_TYPE_2D;
                default:
                R_CORE_ASSERT(false, "");
            }
        }
    };
}
