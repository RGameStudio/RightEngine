#pragma once

#include <cstdint>
#include <glad/glad.h>

namespace RightEngine
{
    class IndexBuffer {
    public:
        IndexBuffer(const uint32_t * data, uint32_t count, int drawMode = GL_STATIC_DRAW);
        ~IndexBuffer();

        void Bind() const;
        void UnBind() const;

        inline unsigned int GetCount() const {
            return count;
        }

    private:
        uint32_t id;
        uint32_t count;
    };
}