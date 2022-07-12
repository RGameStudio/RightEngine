cmake_minimum_required(VERSION 3.19)
#message(${EXECUTABLE_OUTPUT_PATH} ${CMAKE_CURRENT_BINARY_DIR})
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

add_executable(Game Source/Start.cpp
        Source/SandboxLayer.cpp
        Source/Panels/PropertyPanel.cpp
        ../Engine/Source/Core/Loaders/EnvironmentMapLoader.cpp
        Source/Utils/MeshLoader.cpp)

set_target_properties(Game
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})

if (WIN32)
    target_link_libraries(Game Engine glad opengl32 easyargs glfw stb_image imgui assimp)
else()
    target_link_libraries(Game Engine glad easyargs glfw stb_image imgui assimp)
endif()

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