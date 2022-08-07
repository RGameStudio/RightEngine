#pragma once

#include "Assert.hpp"
#include "Types.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <cassert>

namespace RightEngine
{
    struct VertexBufferElement
    {
        Format type;
        uint32_t count;
        bool normalized;

        uint32_t GetSize() const
        { return GetSizeOfType(type) * count; }

        static unsigned int GetSizeOfType(Format format);
    };

    class VertexBufferLayout
    {
    private:
        std::vector<VertexBufferElement> elements;
        uint32_t stride;
    public:
        VertexBufferLayout() : stride(0)
        {

        }

        template<typename T>
        void Push(uint32_t count, bool normalized = false)
        { R_CORE_ASSERT(false, ""); }

        template<typename T>
        void Push()
        { R_CORE_ASSERT(false, ""); }

        inline const std::vector<VertexBufferElement> GetElements() const &
        { return elements; }

        inline unsigned int GetStride() const
        { return stride; }
    };

    template<>
    inline void VertexBufferLayout::Push<float>(uint32_t count, bool normalized)
    {
        elements.push_back({ Format::R32_SFLOAT, count, normalized });
        stride += count * VertexBufferElement::GetSizeOfType(Format::R32_SFLOAT);
    }

    template<>
    inline void VertexBufferLayout::Push<uint32_t>(uint32_t count, bool normalized)
    {
        elements.push_back({ Format::R32_UINT, count, normalized });
        stride += count * VertexBufferElement::GetSizeOfType(Format::R32_UINT);
    }

    template<>
    inline void VertexBufferLayout::Push<uint8_t>(uint32_t count, bool normalized)
    {
        elements.push_back({ Format::R8_UINT, count,  normalized });
        stride += count * VertexBufferElement::GetSizeOfType(Format::R8_UINT);
    }

    template<>
    inline void VertexBufferLayout::Push<glm::vec2>(uint32_t count, bool normalized)
    {
        elements.push_back({ Format::R32G32_SFLOAT, count,  normalized });
        stride += count * VertexBufferElement::GetSizeOfType(Format::R32G32_SFLOAT);
    }

    template<>
    inline void VertexBufferLayout::Push<glm::vec3>(uint32_t count, bool normalized)
    {
        elements.push_back({ Format::R32G32B32_SFLOAT, count,  normalized });
        stride += count * VertexBufferElement::GetSizeOfType(Format::R32G32B32_SFLOAT);
    }

    template<>
    inline void VertexBufferLayout::Push<glm::vec4>(uint32_t count, bool normalized)
    {
        elements.push_back({ Format::R32G32B32A32_SFLOAT, count,  normalized });
        stride += count * VertexBufferElement::GetSizeOfType(Format::R32G32B32A32_SFLOAT);
    }

    template<>
    inline void VertexBufferLayout::Push<glm::vec2>()
    {
        Push<glm::vec2>(1, false);
    }

    template<>
    inline void VertexBufferLayout::Push<glm::vec3>()
    {
        Push<glm::vec3>(1, false);
    }

    template<>
    inline void VertexBufferLayout::Push<glm::vec4>()
    {
        Push<glm::vec4>(1, false);
    }
}