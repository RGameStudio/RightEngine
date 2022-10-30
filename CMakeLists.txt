cmake_minimum_required(VERSION 3.19)
project(RightEngine)

set(CMAKE_CXX_STANDARD 17)

enable_testing()
add_compile_definitions(R_ENABLE_ASSERTS)
if (WIN32)
    add_compile_definitions(R_WIN32)
elseif (APPLE)
    add_compile_definitions(R_APPLE)
endif()

set(GLSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/Engine/Source/Shaders)
set(GLSL_SRC ${GLSL_ROOT_DIR}/Basic/basic.frag
             ${GLSL_ROOT_DIR}/Basic/basic.vert
             ${GLSL_ROOT_DIR}/Basic/pbr.frag
             ${GLSL_ROOT_DIR}/Basic/pbr.vert
             ${GLSL_ROOT_DIR}/Basic/hdr_to_cubemap.frag
             ${GLSL_ROOT_DIR}/Basic/hdr_to_cubemap.vert
             ${GLSL_ROOT_DIR}/Basic/skybox.frag
             ${GLSL_ROOT_DIR}/Basic/skybox.vert
             ${GLSL_ROOT_DIR}/Utils/envmap_to_cubemap.frag
             ${GLSL_ROOT_DIR}/Utils/envmap_to_cubemap.vert
        )
if (WIN32)
    set(Vulkan_LIBRARY $ENV{Vulkan_LIBRARY})
    find_package(Vulkan REQUIRED FATAL_ERROR)
elseif (APPLE)
    find_package(Vulkan REQUIRED FATAL_ERROR)
endif()

message(STATUS "Vulkan include directory: " ${Vulkan_INCLUDE_DIR})
message(STATUS "Vulkan library directory: " ${Vulkan_LIBRARY})

include_directories(${Vulkan_INCLUDE_DIR})
include_directories(Lib/Glad/include)
include_directories(Lib/spdlog/include)
include_directories(Lib/easyargs/include)
include_directories(Lib/googletest/googletest/include)
include_directories(Lib/glfw/include)
include_directories(Lib/glm)
include_directories(Lib/stb_image/Public)
include_directories(Lib/imgui)
include_directories(Lib/entt/include)
include_directories(Lib/assimp/include)
include_directories(${CMAKE_BINARY_DIR}/Lib/assimp/include)
include_directories(Lib/imgui-filebrowser)
include_directories(Lib/glslang/)
include_directories(Lib/vk-tools/)
include_directories(Lib/VulkanMemoryAllocator/include)
include_directories(Lib/crossguid/include)

include_directories(Engine/Source/Public)
include_directories(Engine/Source/Core/Public)
include_directories(Engine/Source/Core/Event/Public)
include_directories(Engine/Source/Core/Launch/Public)
include_directories(Engine/Source/Core/Window/Public)
include_directories(Engine/Source/Core/Application/Public)
include_directories(Engine/Source/Core/RHI/Public)
include_directories(Engine/Source/Core/Testing/Public)
include_directories(Engine/Source/Core/Shader/Public)
include_directories(Engine/Source/Core/Utils/Public)
include_directories(Engine/Source/Core/Scene/Public)
include_directories(Engine/Source/Core/UI/Public)
include_directories(Engine/Source/Core/Loaders/Public)

#include(Lib/glslang/CMakeLists.txt)

add_subdirectory(Engine)
add_subdirectory(Game)
add_subdirectory(Tests)
add_subdirectory(Sandbox)
add_subdirectory(Lib/spdlog)
add_subdirectory(Lib/Glad)
add_subdirectory(Lib/googletest)
add_subdirectory(Lib/easyargs)
add_subdirectory(Lib/glfw)
add_subdirectory(Lib/glm)
add_subdirectory(Lib/stb_image)
add_subdirectory (Lib/imgui)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Build the assimp viewing tools")
add_subdirectory(Lib/assimp)
add_subdirectory(Lib/glslang)
add_subdirectory(Lib/vk-tools/)
add_subdirectory(Lib/VulkanMemoryAllocator)
add_subdirectory(Lib/crossguid)