#pragma once

#include "Texture.hpp"
#include "Buffer.hpp"
#include "ShaderProgramDescriptor.hpp"
#include "Utils.hpp"
#include <unordered_map>

namespace RightEngine
{
    struct BufferRef
    {
        int slot;
        ShaderStage stage;

        bool operator==(const BufferRef& other) const
        {
            return slot == other.slot && stage == other.stage;
        }
    };

    struct BufferRefHash
    {
        size_t operator()(const BufferRef& p) const
        {
            size_t h = 0x1231;
            Utils::CombineHash(h, p.slot);
            Utils::CombineHash(h, p.stage);

            return h;
        }
    };

    class RendererState
    {
    public:
        const std::shared_ptr<Texture>& GetTexture(int slot);
        void SetTexture(const std::shared_ptr<Texture>& texture, int slot);

        const std::shared_ptr<Buffer>& GetFragmentBuffer(int slot);
        void SetFragmentBuffer(const std::shared_ptr<Buffer>& buffer, int slot);

        const std::shared_ptr<Buffer>& GetVertexBuffer(int slot);
        void SetVertexBuffer(const std::shared_ptr<Buffer>& buffer, int slot);

        virtual void OnUpdate() = 0;

    protected:
        std::unordered_map<int, std::shared_ptr<Texture>> textures;
        std::unordered_map<BufferRef, std::shared_ptr<Buffer>, BufferRefHash> buffers;

        bool isSyncNeeded{ false };
        std::vector<std::pair<BufferRef, std::weak_ptr<Buffer>>> buffersToSync;
        std::vector<std::pair<int, std::weak_ptr<Texture>>> texturesToSync;

    private:
        void SetBuffer(const std::shared_ptr<Buffer>& buffer, int slot, ShaderStage stage);
    };
}
