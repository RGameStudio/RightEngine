cmake_minimum_required(VERSION 3.19)

add_executable(Sandbox Source/Start.cpp Source/SandboxLayer.cpp)

set_target_properties(Sandbox
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(Sandbox
        Engine
        glad
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
)

if (WIN32)
target_link_libraries(Sandbox opengl32)
elseif (APPLE)
endif()

add_custom_target(CopyShaderFilesToSandbox ALL DEPENDS ${GLSL_ROOT_DIR})

add_custom_command(
        TARGET CopyShaderFilesToSandbox
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Engine/Source/Shaders
        ${CMAKE_CURRENT_BINARY_DIR}/Assets/Shaders
        DEPENDS ${GLSL_SRC})

add_dependencies(Sandbox CopyShaderFilesToSandbox)

add_custom_target(CopyResourceFilesToSandbox ALL DEPENDS ${ASSETS_DIR})

add_custom_command(
        TARGET CopyResourceFilesToSandbox
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Sandbox/Resources
        ${CMAKE_CURRENT_BINARY_DIR}/Assets/
        DEPENDS ${ASSETS_DIR})

add_dependencies(Sandbox CopyResourceFilesToSandbox)