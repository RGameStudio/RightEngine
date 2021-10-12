#pragma once

#include <cstdint>

namespace RightEngine
{
    class IndexBuffer {
    public:
        IndexBuffer(const unsigned int * data, unsigned int count, int drawMode = GL_STATIC_DRAW);
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