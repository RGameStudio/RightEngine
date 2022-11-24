cmake_minimum_required(VERSION 3.19)

set(ENGINE_SRC
        Source/Core/Logger.cpp
        Source/Core/Launch/LaunchEngine.cpp
        Source/Core/Event/EventDispatcher.cpp
        Source/Core/Window/Window.cpp
        Source/Core/Window/WindowsWindow.cpp
        Source/Core/Application/Application.cpp
        Source/Core/RHI/VertexBufferLayout.cpp
        Source/Core/RHI/Renderer.cpp
        Source/Core/Utils/Input.cpp
        Source/Core/Window/GLFWWindow.cpp
        Source/Core/Scene/Camera.cpp
        Source/Core/Scene/Entity.cpp
        Source/Core/Scene/Scene.cpp
        Source/Core/RHI/Material.cpp
        Source/Core/Utils/Path.cpp
        Source/Core/Utils/MeshBuilder.cpp
        Source/Core/Application/Layer.cpp
        Source/Core/RHI/RendererCommand.cpp
        Source/Core/RHI/RendererAPI.cpp
        Source/Core/UI/ImGuiLayer.cpp
        Source/Core/UI/ImGuiBuild.cpp
        Source/Core/Scene/Components/Transform.cpp
        Source/Core/Loaders/TextureLoader.cpp
        Source/Core/RHI/Sampler.cpp
        Source/Core/Utils/String.cpp
        Source/Core/Loaders/AssetManager.cpp
        Source/Core/Loaders/EnvironmentMapLoader.cpp
        Source/Core/Loaders/MeshLoader.cpp
        Source/Core/RHI/Vulkan/VulkanRenderingContext.cpp
        Source/Core/RHI/Vulkan/VulkanRendererAPI.cpp
        Source/Core/RHI/Device.cpp
        Source/Core/RHI/Vulkan/VulkanDevice.cpp
        Source/Core/RHI/Vulkan/VulkanSwapchain.cpp
        Source/Core/RHI/Vulkan/VulkanShader.cpp
        Source/Core/RHI/Vulkan/VulkanSurface.cpp
        Source/Core/RHI/Vulkan/VulkanGraphicsPipeline.cpp
        Source/Core/RHI/Vulkan/VulkanBuffer.cpp
        Source/Core/RHI/Vulkan/VulkanUtils.cpp
        Source/Core/RHI/Vulkan/VulkanCommandBuffer.cpp
        Source/Core/RHI/Vulkan/VulkanTexture.cpp
        Source/Core/RHI/Vulkan/VulkanSampler.cpp
        Source/Core/RHI/RendererState.cpp
        Source/Core/RHI/Vulkan/VulkanRendererState.cpp
        Source/Core/RHI/Vulkan/VulkanImguiLayerImpl.cpp
        Source/Core/Loaders/AssetLoader.cpp)

add_library(Engine STATIC ${ENGINE_SRC})
target_compile_definitions(Engine PUBLIC OPENGL_MAJOR_VERSION=4 OPENGL_MINOR_VERSION=5)

add_library(EngineTest STATIC ${ENGINE_SRC})
target_compile_definitions(EngineTest PUBLIC R_TEST_BUILD OPENGL_MAJOR_VERSION=3 OPENGL_MINOR_VERSION=3)

if (WIN32)
    add_compile_definitions(WIN32)
elseif(APPLE)
    add_compile_definitions(APPLE)
endif()