cmake_minimum_required(VERSION 3.19)

set(ENGINE_SRC
        Source/Core/Logger.cpp
        Source/Core/Launch/LaunchEngine.cpp
        Source/Core/Event/EventDispatcher.cpp
        Source/Core/Window/Window.cpp
        Source/Core/Window/WindowsWindow.cpp
        Source/Core/Application/Application.cpp
        Source/Core/RHI/WindowsRenderingContext.cpp
        Source/Core/RHI/VertexBuffer.cpp
        Source/Core/RHI/VertexBufferLayout.cpp
        Source/Core/RHI/VertexArray.cpp
        Source/Core/RHI/IndexBuffer.cpp
        Source/Core/RHI/Renderer.cpp
        Source/Core/RHI/DebugRHI.cpp
        Source/Core/RHI/Shader.cpp
        Source/Core/Utils/Input.cpp
        Source/Core/Window/GLFWWindow.cpp
        Source/Core/Scene/Camera.cpp
        Source/Core/Scene/Entity.cpp
        Source/Core/Scene/Scene.cpp
        Source/Core/RHI/Material.cpp
        Source/Core/Utils/Path.cpp
        Source/Core/Utils/MeshBuilder.cpp
        Source/Core/RHI/FrameBuffer.cpp
        Source/Core/Application/Layer.cpp
        Source/Core/RHI/OpenGL/OpenGLRendererAPI.cpp
        Source/Core/RHI/RendererCommand.cpp
        Source/Core/RHI/RendererAPI.cpp
        Source/Core/UI/ImGuiLayer.cpp
        Source/Core/UI/ImGuiBuild.cpp
        Source/Core/Scene/EditorCamera.cpp
        Source/Core/Scene/Components/Transform.cpp
        Source/Core/Scene/Components/Mesh.cpp
        Source/Core/RHI/OpenGL/OpenGLShader.cpp
        Source/Core/RHI/UniformBuffer.cpp
        Source/Core/RHI/OpenGL/OpenGLUniformBuffer.cpp
        Source/Core/RHI/OpenGL/OpenGLTexture.cpp
        Source/Core/RHI/Texture.cpp
        Source/Core/Loaders/TextureLoader.cpp
        Source/Core/RHI/Sampler.cpp
        Source/Core/RHI/OpenGL/OpenGLSampler.cpp
        Source/Core/RHI/OpenGL/OpenGLConverters.cpp
        Source/Core/Utils/String.cpp
        Source/Core/Scene/AssetManager.cpp
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
        Source/Core/RHI/Public/Buffer.hpp Source/Core/RHI/Vulkan/VulkanBuffer.cpp Source/Core/RHI/Vulkan/VulkanBuffer.hpp Source/Core/RHI/Public/BufferDescriptor.hpp Source/Core/RHI/Public/ShaderProgramDescriptor.hpp)

add_library(Engine STATIC ${ENGINE_SRC})
target_compile_definitions(Engine PUBLIC OPENGL_MAJOR_VERSION=4 OPENGL_MINOR_VERSION=5)

add_library(EngineTest STATIC ${ENGINE_SRC})
target_compile_definitions(EngineTest PUBLIC R_TEST_BUILD OPENGL_MAJOR_VERSION=3 OPENGL_MINOR_VERSION=3)

if (WIN32)
    add_compile_definitions(WIN32)
elseif(APPLE)
    add_compile_definitions(APPLE)
endif()