#pragma once

#include "Assert.hpp"
#include "AssetBase.hpp"
#include "Sampler.hpp"
#include "Device.hpp"
#include "TextureDescriptor.hpp"
#include <cstdint>
#include <string>
#include <memory>

namespace RightEngine
{
    class Texture : public AssetBase
    {
    public:
        ASSET_BASE()

        virtual ~Texture() = default;

        inline int GetWidth() const
        { return specification.width; }

        inline int GetHeight() const
        { return specification.height; }

        inline uint32_t GetId() const
        { return id; }

        inline std::shared_ptr<Sampler> GetSampler() const
        { return sampler; }

        void SetSampler(const std::shared_ptr<Sampler>& aSampler)
        { sampler = aSampler; }

        inline const TextureDescriptor& GetSpecification() const
        { return specification; }

        virtual void GenerateMipmaps() const = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void UnBind() const = 0;

        static std::shared_ptr<Texture> Create(const TextureDescriptor& aSpecification,
                                               const std::vector<uint8_t>& data)
        {
            R_CORE_ASSERT(false, "");
            return nullptr;
        }

        static std::shared_ptr<Texture> Create(const TextureDescriptor& aSpecification,
                                               const CubemapFaces& faces)
        {
            R_CORE_ASSERT(false, "");
            return nullptr;
        }

    protected:
        Texture(const std::shared_ptr<Device>& device,
                const TextureDescriptor& aSpecification,
                const std::vector<uint8_t>& data) : specification(aSpecification)
        {}

        uint32_t id;
        TextureDescriptor specification;
        std::shared_ptr<Sampler> sampler;
    };
}