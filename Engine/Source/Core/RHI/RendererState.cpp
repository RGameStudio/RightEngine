#include "RendererState.hpp"

using namespace RightEngine;

namespace
{
    const std::shared_ptr<Buffer>& GetBuffer(const std::unordered_map<BufferRef, std::shared_ptr<Buffer>,
            BufferRefHash>& buffers, int slot, ShaderStage stage)
    {
        const auto bufferIt = buffers.find({ slot, stage });
        if (bufferIt == buffers.end())
        {
//            R_CORE_ASSERT(false, "");
            return nullptr;
        }

        return bufferIt->second;
    }
}

const std::shared_ptr<Texture>& RendererState::GetTexture(int slot)
{
    const auto textureIt = textures.find({ slot });
    if (textureIt == textures.end())
    {
//        R_CORE_ASSERT(false, "");
        return nullptr;
    }
    return textureIt->second;
}

void RendererState::SetTexture(const std::shared_ptr<Texture>& texture, int slot)
{
    R_CORE_ASSERT(texture, "");
    textures[slot] = texture;
    isSyncNeeded = true;
    texturesToSync.emplace_back( slot, texture );
}

const std::shared_ptr<Buffer>& RendererState::GetFragmentBuffer(int slot)
{
    return GetBuffer(buffers, slot, ShaderType::FRAGMENT);
}

void RendererState::SetFragmentBuffer(const std::shared_ptr<Buffer>& buffer, int slot, int offset, int stride)
{
    SetBuffer(buffer, slot, ShaderType::FRAGMENT, offset, stride);
}

const std::shared_ptr<Buffer>& RendererState::GetVertexBuffer(int slot)
{
    return GetBuffer(buffers, slot, ShaderType::VERTEX);
}

void RendererState::SetVertexBuffer(const std::shared_ptr<Buffer>& buffer, int slot, int offset, int stride)
{
    SetBuffer(buffer, slot, ShaderType::VERTEX, offset, stride);
}

void RendererState::SetBuffer(const std::shared_ptr<Buffer>& buffer, int slot, ShaderStage stage, int offset, int stride)
{
    R_CORE_ASSERT(buffer, "");

    BufferRef ref{ slot, stage };
    buffers[ref] = buffer;
    isSyncNeeded = true;
    buffersToSync.emplace_back( ref, buffer );

    if (stride != 0)
    {
        offsets[ref] = { offset, stride };
    }
}
