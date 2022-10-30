cmake_minimum_required(VERSION 3.19)

add_executable(Game Source/Start.cpp
        Source/SandboxLayer.cpp
        Source/Panels/PropertyPanel.cpp)

set_target_properties(Game
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(Game
        Engine
        easyargs
        glfw
        stb_image
        imgui
        assimp
        glslang
        MachineIndependent
        GenericCodeGen
        OGLCompiler
        OSDependent
        SPIRV
        ${Vulkan_LIBRARY}
        glslang-default-resource-limits
        vk-tools
        VulkanMemoryAllocator
        crossguid)

add_custom_target(CopyShaderFilesToGame ALL DEPENDS ${GLSL_ROOT_DIR})

add_custom_command(
        TARGET CopyShaderFilesToGame
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Engine/Source/Shaders
        ${CMAKE_CURRENT_BINARY_DIR}/Assets/Shaders
        DEPENDS ${GLSL_SRC})

add_dependencies(Game CopyShaderFilesToGame)

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/Game/Resources)

add_custom_target(CopyResourceFilesToGame ALL DEPENDS ${ASSETS_DIR})

add_custom_command(
        TARGET CopyResourceFilesToGame
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSETS_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/Assets/
        DEPENDS ${ASSETS_DIR})

add_dependencies(Game CopyResourceFilesToGame)

if (WIN32)
#    add_compile_definitions(WIN32)
elseif(APPLE)
    add_compile_definitions(APPLE)
endif()