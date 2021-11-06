cmake_minimum_required(VERSION 3.19)

add_executable(Game Source/Start.cpp)
target_link_libraries(Game Engine glad opengl32 easyargs)

add_custom_target(CopyShaderFilesToGame ALL DEPENDS ${GLSL_ROOT_DIR})

add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/Engine/Source/Shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Engine/Source/Shaders
        ${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders
        DEPENDS ${GLSL_SRC})

add_dependencies(Game CopyShaderFilesToGame)