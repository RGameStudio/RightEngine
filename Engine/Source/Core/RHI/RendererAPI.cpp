#include "RendererAPI.hpp"
#include "Assert.hpp"
#include "OpenGL/OpenGLRendererAPI.hpp"
#include "Vulkan/VulkanRendererAPI.hpp"

using namespace RightEngine;

std::shared_ptr<RendererAPI> RendererAPI::Create(GPU_API GpuApi)
{
    api = GpuApi;
    switch (GpuApi)
    {
        case GPU_API::OpenGL:
            return std::make_shared<OpenGLRendererAPI>();
        case GPU_API::Vulkan:
            return std::make_shared<VulkanRendererAPI>();
        default:
            R_CORE_ASSERT(false, "Unknown GPU API!");
    }
}

GPU_API RendererAPI::GetAPI()
{
    return api;
}

GPU_API RendererAPI::api = GPU_API::None;
