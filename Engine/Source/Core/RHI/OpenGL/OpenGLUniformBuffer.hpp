#pragma once

#include "UniformBuffer.hpp"

namespace RightEngine
{
    class OpenGLUniformBuffer : public UniformBuffer
    {
    public:
        OpenGLUniformBuffer(uint32_t size, uint32_t binding);
        virtual ~OpenGLUniformBuffer() override;

        virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;

    private:
        uint32_t id;
    };
}
