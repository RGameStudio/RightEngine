cmake_minimum_required(VERSION 3.19)

set(ENGINE_SRC
        Source/Core/Logger.cpp
        Source/Core/Launch/LaunchEngine.cpp
        Source/Core/Event/EventDispatcher.cpp
        Source/Core/Window/Window.cpp
        Source/Core/Window/WindowsWindow.cpp
        Source/Core/Application/Application.cpp
        Source/Core/RHI/RenderingContext.cpp
        Source/Core/RHI/VertexBuffer.cpp
        Source/Core/RHI/Public/VertexBufferLayout.cpp
        Source/Core/RHI/VertexArray.cpp
        Source/Core/RHI/IndexBuffer.cpp
        Source/Core/RHI/Renderer.cpp
        Source/Core/RHI/DebugRHI.cpp
        Source/Core/Shader/Shader.cpp
        Source/Core/Utils/Input.cpp
        )

add_library(Engine STATIC ${ENGINE_SRC})
target_compile_definitions(Engine PUBLIC OPENGL_MAJOR_VERSION=4 OPENGL_MINOR_VERSION=5)

add_library(EngineTest STATIC ${ENGINE_SRC})
target_compile_definitions(EngineTest PUBLIC R_TEST_BUILD OPENGL_MAJOR_VERSION=3 OPENGL_MINOR_VERSION=3)