cmake_minimum_required(VERSION 3.19)

add_executable(Game Source/Start.cpp)
if (WIN32)
    target_link_libraries(Game Engine glad opengl32 easyargs glfw)
else()
    target_link_libraries(Game Engine glad easyargs glfw)
endif()

add_custom_target(CopyShaderFilesToGame ALL DEPENDS ${GLSL_ROOT_DIR})

add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/Engine/Source/Shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Engine/Source/Shaders
        ${CMAKE_CURRENT_BINARY_DIR}/Resources/Shaders
        DEPENDS ${GLSL_SRC})

add_dependencies(Game CopyShaderFilesToGame)

if (WIN32)
    add_compile_definitions(WIN32)
elseif(APPLE)
    add_compile_definitions(APPLE)
endif()