cmake_minimum_required(VERSION 3.19)

set(ENGINE_SRC Source/Core/Public/Logger.hpp
        Source/Core/Logger.cpp Source/Core/Launch/LaunchEngine.cpp
        Source/Core/Launch/Public/LaunchEngine.hpp
        Source/Core/Event/Public/Event.hpp
        Source/Core/Event/Public/DemoEvent.hpp
        Source/Core/Event/EventDispatcher.cpp
        Source/Core/Event/Public/EventDispatcher.hpp
        Source/Core/Event/Public/ShutdownEvent.hpp
        Source/Core/Window/Public/Window.hpp
        Source/Core/Window/Window.cpp
        Source/Core/Window/WindowsWindow.cpp
        Source/Core/Window/WindowsWindow.hpp
        Source/Core/Application/Public/Application.hpp
        Source/Core/Application/Application.cpp
        Source/Core/RHI/Public/RenderingContext.hpp
        Source/Core/RHI/RenderingContext.cpp
        Source/Core/RHI/VertexBuffer.cpp
        Source/Core/RHI/Public/VertexBuffer.hpp
        Source/Core/RHI/Public/VertexBufferLayout.hpp
        Source/Core/RHI/Public/VertexBufferLayout.cpp
        Source/Core/RHI/Public/VertexArray.hpp
        Source/Core/RHI/VertexArray.cpp
        Source/Core/RHI/Public/IndexBuffer.hpp
        Source/Core/RHI/IndexBuffer.cpp
        Source/Core/RHI/Public/Renderer.hpp
        Source/Core/RHI/Renderer.cpp
        Source/Core/RHI/Public/DebugRHI.hpp
        Source/Core/RHI/DebugRHI.cpp Source/Core/Shader/Shader.cpp Source/Core/Shader/Public/Shader.hpp)

add_library(Engine STATIC ${ENGINE_SRC})
target_compile_definitions(Engine PUBLIC OPENGL_MAJOR_VERSION=4 OPENGL_MINOR_VERSION=5)

add_library(EngineTest STATIC ${ENGINE_SRC})
target_compile_definitions(EngineTest PUBLIC R_TEST_BUILD OPENGL_MAJOR_VERSION=3 OPENGL_MINOR_VERSION=3)