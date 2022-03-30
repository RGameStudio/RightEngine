#include "RendererAPI.hpp"
#include "OpenGL/OpenGLRendererAPI.hpp"
#include "Assert.hpp"

using namespace RightEngine;

std::shared_ptr<RendererAPI> RendererAPI::Create(GPU_API api)
{
    switch (api)
    {
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLRendererAPI>();
        default:
            R_CORE_ASSERT(false, "Unknown GPU API!");
    }
}
