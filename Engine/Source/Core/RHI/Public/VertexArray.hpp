#include <cstdint>
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

namespace RightEngine
{
    class VertexArray
    {
    public:
        VertexArray();

        ~VertexArray();

        void Bind() const;

        void UnBind() const;

        void AddBuffer(VertexBuffer vb, VertexBufferLayout layout);
    private:
        uint32_t id;
        VertexBuffer buffer;
        VertexBufferLayout layout;
    };
}
